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
#include "Constants.h"
#include "Helpers.h"
#include "Convert.h"
#include "Input.h"
#include "Drawer.h"
#include "ProjectionFitter.h"
#include "CalibrationFunctionFitter.h"
#include <iostream>
using std::vector;
using std::cout;
using std::endl;



void calculateCalibrationFunctions(const char* trb3dir, const char* subdir, int wall){

	// ---------------- Import all correlation plots showing ToT vs TDiff ----------------
	vector<Module> modules(Constants::nModules);
	for(int iMod = 0; iMod < Constants::nModules; iMod++){
		modules[iMod].maxGraph.SetName("graph_module" + Convert::toStr(iMod));
		modules[iMod].setHorizontal(Helpers::isHorizontal(wall, iMod));
		modules[iMod].iMod = iMod;
	}
	Input input(TString(trb3dir) + "/data/trackingForEnergyCalibration/" + TString(subdir), modules);

	double energy_at_which_to_calculate_dEPerE = 10.;
	Results results;

	// print information for one of the modules
	vector<int> modulesToPrint = {8, 32};

	

	// calibrate all modules
	for(int iMod = 0; iMod < modules.size(); iMod++){

		if(!modules[iMod].hEDepVsTot.GetEntries()) continue;

		ProjectionFitter projectionFitter;

		projectionFitter.fit(modules[iMod]);

		modules[iMod].createCalibrationFunction();

		CalibrationFunctionFitter calFuncFitter;
		calFuncFitter.setParameters(modules[iMod]);
		calFuncFitter.fit(modules[iMod]);		
		
		results.saveFitResult(modules[iMod], energy_at_which_to_calculate_dEPerE);

		if(Helpers::vectorContains(modulesToPrint, iMod)){
			modules[iMod].print();
		}
	}

	// write 
	TString pathOut(trb3dir);
	if(!pathOut.EndsWith("/")) pathOut += "/";
	pathOut += "data/energyCalibration/";
	pathOut += subdir + TString("/calibration.root");

	TFile *fileOut = new TFile(pathOut, "recreate");
	
	for(int iMod = 0; iMod < Constants::nModules; iMod++){
		if(!modules[iMod].hEDepVsTot.GetEntries()) continue;
		modules[iMod].calibrationFunction->Write();
		modules[iMod].hEDepVsTot.Write();
		modules[iMod].maxGraph.Write();
		results.hEnergyResolution.Write();
		for(int iPar = 0; iPar < 3; iPar++){
			results.hVecFitPar[iPar].Write();
		}
	}

	// draw
	Drawer dr;
	dr.drawOverview(results);
	dr.drawModules(modules);
}