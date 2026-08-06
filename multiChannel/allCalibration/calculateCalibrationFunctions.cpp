/*
	HIMEana: Analyze HIME data.
	
	Copyright (C) 2023, 2024 Marco Knösel (mknoesel@ikp.tu-darmstadt.de)

	This file is part of HIMEana.
	
	HIMEana is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	HIMEana is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with HIMEana.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "calculateCalibrationFunctions.h"
#include "Input.h"
#include "Drawer.h"
#include "Fit.h"
#include "TSyncSolver.h"
#include <iostream>
#include <vector>
using std::vector;
using std::cout;
using std::endl;

//     _______________
//     | .       ~   |
//     |  HIME ~   . |
//     | .~  rocks  .|
//     |_____________|
//            |
//      |\/\  |
//      (^.^) |
//     ,/   \-o
//  ----------------------------  



void calculateCalibrationFunctions(TString trb3dir, TString subdir, int nCountsPerProjection, int nProjections){

	// ---------------- Input ----------------
	//TString path(trb3dir + "/data/trackingForAllCalibration/" + subdir + ".root");
	TString path(trb3dir + "/calibration/trackingForAllCalibration/" + subdir + ".root");
	vector<Module> modules;

	cout << "[calculateCalibrationFunctions] Reading file " << path.Data() << endl;
	Input input(path, modules);
	
	// ---------------- Fit ----------------
	Fit f(nCountsPerProjection, nProjections);
	f.perform(modules);

	// ---------------- TSync----------------
	TSyncSolver s;
	s.solve(modules);

	// ---------------- Fill histograms ----------------
	HistogramCollection hc;
	hc.fill(modules);

	// ---------------- Write ----------------
	//TFile* fileOut = new TFile(trb3dir + "/data/calibrationParameter/" + subdir + "/calibration.root", "recreate");
	TFile* fileOut = new TFile(trb3dir + "/calibration/calibrationParameter/" + subdir + "/calibration.root", "recreate");
	hc.write(fileOut);
	for(const Module& m: modules) m.write(fileOut);

	//cout << "[calculateCalibrationFunctions] Calibration File witten to " << trb3dir + "/data/calibrationParameter/" + subdir + "/calibration.root" << endl;
	cout << "[calculateCalibrationFunctions] Calibration File witten to " << trb3dir + "/calibration/calibrationParameter/" + subdir + "/calibration.root" << endl;

	// ---------------- Draw ----------------
	//Drawer dr;
	//dr.drawOverview(hc);
	//dr.drawModules(modules);
}
