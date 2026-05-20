#include "TSyncSolver.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
#include <iostream>
#include <cmath>
#include <limits>
using std::cout;
using std::endl;

void TSyncSolver::solve(std::vector<Module>& modules)
{
    struct Equation {
        int bar1, bar2;
        double value, weight;
    };

    auto solveSubSystem = [&](int startBar, int nBars, int nLayers)
    {
        // first collect all equations without active check
        std::vector<Equation> allEquations;
        for(int id = 0; id < nBars; id++) {
            Module& m = modules[id + startBar];
            int layer = id / Constants::nModulesPerLayer;

            // NextBar equations
            int nextID = id + 1;
            if(!std::isnan(m.tSyncNextBar.Value) &&
               !std::isnan(m.tSyncNextBar.Error) &&
               m.tSyncNextBar.Error > 0. &&
               (id + 1) % Constants::nModulesPerLayer != 0 &&
               nextID < nBars) {
                allEquations.push_back({
                    id,
                    nextID,
                    m.tSyncNextBar.Value,
                    1.0 / m.tSyncNextBar.Error
                });
            }

            // NextPlane equations
            if(layer < nLayers - 1) {
                for(int ob = 0; ob < Constants::nModulesPerLayer; ob++) {
                    int otherID = (layer + 1) * Constants::nModulesPerLayer + ob;
                    if(otherID >= nBars) continue;
                    if(!std::isnan(m.tSyncNextPlane[ob].Value) &&
                       !std::isnan(m.tSyncNextPlane[ob].Error) &&
                       m.tSyncNextPlane[ob].Error > 0.) {
                        allEquations.push_back({
                            id,
                            otherID,
                            m.tSyncNextPlane[ob].Value,
                            1.0 / m.tSyncNextPlane[ob].Error
                        });
                    }
                }
            }
        }

        // determine active bars from equations
        // a bar is active if it appears in ANY equation as bar1 OR bar2
        std::vector<bool> isActive(nBars, false);
        for(auto& eq : allEquations) {
            isActive[eq.bar1] = true;
            isActive[eq.bar2] = true;
        }

        // report truly dead bars
        for(int id = 0; id < nBars; id++) {
            if(!isActive[id])
                cout << "[TSyncSolver] Excluding dead bar " 
                     << id + startBar << endl;
        }

        // create index mapping from full to reduced
        std::vector<int> barToReduced(nBars, -1);
        std::vector<int> reducedToBar;
        int nActive = 0;
        for(int id = 0; id < nBars; id++) {
            if(isActive[id]) {
                barToReduced[id] = nActive++;
                reducedToBar.push_back(id);
            }
        }

        cout << "[TSyncSolver] " << nBars - nActive 
             << " dead bars excluded, " << nActive 
             << " active bars." << endl;

        // filter equations to only include active bars
        std::vector<Equation> equations;
        for(auto& eq : allEquations) {
            if(isActive[eq.bar1] && isActive[eq.bar2]) {
                equations.push_back({
                    barToReduced[eq.bar1],
                    barToReduced[eq.bar2],
                    eq.value,
                    eq.weight
                });
            }
        }

        if((int)equations.size() < nActive) {
            cout << "[TSyncSolver] Not enough equations ("
                 << equations.size() << " for " << nActive
                 << " active bars). Aborting." << endl;
            return;
        }

        cout << "[TSyncSolver] Solving with " << equations.size()
             << " equations for " << nActive << " active bars." << endl;

        // +1 for sum=0 constraint
        const int nEq = equations.size() + 1;
        TMatrixD A(nEq, nActive);
        TVectorD b(nEq);
        A.Zero();
        b.Zero();

        // fill weighted equations
        for(int r = 0; r < (int)equations.size(); r++) {
            auto& eq = equations[r];
            A(r, eq.bar1) = -eq.weight;
            A(r, eq.bar2) = +eq.weight;
            b(r)          =  eq.value * eq.weight;
        }

        // sum = 0 constraint
        for(int col = 0; col < nActive; col++)
            A(nEq-1, col) = 1.0;
        b(nEq-1) = 0.;

        // solve with SVD
        TDecompSVD svd(A);
        bool ok;
        TVectorD solution = svd.Solve(b, ok);

        if(!ok) {
            cout << "[TSyncSolver] SVD solve failed!" << endl;
            return;
        }

        // store results back into modules
        for(int id = 0; id < nBars; id++) {
            if(isActive[id]) {
                modules[id + startBar].tSync = {
                    solution(barToReduced[id]), 0.};
            } else {
                modules[id + startBar].tSync = {
                    std::numeric_limits<double>::quiet_NaN(), 0.};
                cout << "[TSyncSolver] Dead bar " << id + startBar
                     << " gets NaN tSync." << endl;
            }
            cout << "[TSyncSolver] Bar " << id + startBar 
                 << "\t tSync = " << modules[id + startBar].tSync.Value 
                 << endl;
        }
    };

    // solve HIME+
    cout << "[TSyncSolver] ---- Solving HIME+ ----" << endl;
    const int startHimeP = 0;
    const int nBarsHimeP = Constants::nLayersPerWallHimeP * Constants::nModulesPerLayer;
    solveSubSystem(startHimeP, nBarsHimeP, Constants::nLayersPerWallHimeP);

    // solve HIME
    cout << "[TSyncSolver] ---- Solving HIME ----" << endl;
    const int startHime = nBarsHimeP;
    const int nBarsHime = Constants::nModules - startHime;
    solveSubSystem(startHime, nBarsHime, Constants::nLayersPerWall);
}
