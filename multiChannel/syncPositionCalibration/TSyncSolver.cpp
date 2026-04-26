#include "TSyncSolver.h"
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TDecompSVD.h"
#include <iostream>
#include <cmath>
using std::cout;
using std::endl;

void TSyncSolver::solve(std::vector<Module>& modules) {

    const int nBars = Constants::nModules;
    
    struct Equation {
        int bar1, bar2;
        double value, weight;
    };
    std::vector<Equation> equations;

    for(int id = 0; id < nBars; id++) {
        Module& m = modules[id];
        int layer = id / Constants::nModulesPerLayer;

        // NextBar equations - check not last bar in layer
        if(!std::isnan(m.tSyncNextBar.Value) &&
           !std::isnan(m.tSyncNextBar.Error) &&
           m.tSyncNextBar.Error > 0. &&
           (id+1) % Constants::nModulesPerLayer != 0) {
            equations.push_back({
                id, 
                id + 1,
                m.tSyncNextBar.Value,
                1.0 / m.tSyncNextBar.Error
            });
        }

        // NextPlane equations - check not last layer
        if(layer < Constants::nLayersPerWall - 1) {
            for(int ob = 0; ob < Constants::nModulesPerLayer; ob++) {
                int otherID = (layer + 1) * Constants::nModulesPerLayer + ob;
                if(!std::isnan(m.tSyncNextPlane[ob].Value) &&
                   !std::isnan(m.tSyncNextPlane[ob].Error) &&
                   m.tSyncNextPlane[ob].Error > 0.) {
                    equations.push_back({
                        id,
                        otherID,
                        m.tSyncNextPlane[ob].Value,
                        1.0 / m.tSyncNextPlane[ob].Error
                    });
                }
            }
        }
    }

    if(equations.size() < nBars) {
        cout << "[TSyncSolver] Not enough equations (" 
             << equations.size() << " for " << nBars << " bars). Aborting." << endl;
        return;
    }

    cout << "[TSyncSolver] Solving with " << equations.size() << " equations for " 
         << nBars << " unknowns." << endl;

    // +1 for sum=0 constraint
    const int nEq = equations.size() + 1;

    TMatrixD A(nEq, nBars);
    TVectorD b(nEq);
    A.Zero();
    b.Zero();

    // fill weighted equations: weight * (TSync_bar2 - TSync_bar1) = weight * value
    for(int r = 0; r < (int)equations.size(); r++) {
        auto& eq = equations[r];
        A(r, eq.bar1) = -eq.weight;
        A(r, eq.bar2) = +eq.weight;
        b(r)          =  eq.value * eq.weight;
    }

    // sum = 0 constraint (pins global offset)
    for(int col = 0; col < nBars; col++)
        A(nEq-1, col) = 1.0;
    b(nEq-1) = 0.;

    // solve with SVD least squares
    TDecompSVD svd(A);
    bool ok;
    TVectorD solution = svd.Solve(b, ok);

    if(!ok) {
        cout << "[TSyncSolver] SVD solve failed!" << endl;
        return;
    }

    // store results back into modules
    std::vector<double> result(nBars);
    for(int i = 0; i < nBars; i++) {
        result[i] = solution(i);
        modules[i].tSync = {solution(i), 0.}; // error not available from SVD directly
    }
}
