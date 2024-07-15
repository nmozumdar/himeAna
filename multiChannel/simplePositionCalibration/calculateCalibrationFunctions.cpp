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
#include "Fitter.h"
#include "Input.h"
#include "Thresholds.h"
#include "Drawer.h"
#include <iostream>

using std::array;
using std::vector;
using std::to_string;
using std::cout;
using std::endl;



void calculateCalibrationFunctions(const char* trb3dir, const char* subdir, const char* thresholdsFileName, vector<const char*> filenames){


	// ---------------- Import all correlation plots showing ToT vs TDiff ----------------

	vector<Module> modules(Constants::nModules);
	// set the names of the ToT-vs.-tDiff correlation plots and their projections
	for(int id = 0; id < Constants::nModules; id++) modules[id].setID(id);
	
	Input input(trb3dir, subdir, filenames, modules);


	// ---------------- Import thresholds ----------------

	TString pathToCSVFile(trb3dir);
	pathToCSVFile += TString("/data/thresholds/") + thresholdsFileName;
	Thresholds thrs(pathToCSVFile, Constants::nModules);


	// ---------------- Loop over modules: Determine calibration functions ----------------

	// save the results in histograms
	HistogramCollection hc;

	for(int mod = 0; mod < Constants::nModules; mod++){
		
		// check if the current module was in operation
		if(!modules[mod].hTotVsTDiff.GetEntries()) continue; 
		
		// Determine the edges of the tDiff distribution and their uncertainties.
		// The position-calibration function is calculated immediately from the result in class Module.
		Fitter::determineEdges(modules[mod], thrs[mod]);

		// fill histograms 
		hc.fillHistograms(modules[mod], thrs[mod]);

		// print effective velocities in the terminal window
		const float invC = 1. / 299.792458;
		cout << "[calculateCalibrationFunctions] Module " << mod << ": v_eff = (" << modules[mod].getVEff() * invC << " +- " << modules[mod].getVEffUnc() * invC << ") c" << endl;
	}

	// ---------------- write output ----------------

	TString directory = TString(trb3dir) + "/data/simplePositionCalibration/" + TString(subdir) + "/";
	TString infoPathOut = directory + "info.root";
	TString calibrationPathOut = directory + "calibration.root";

	cout << "[calculateCalibrationFunctions] Writing info file " << infoPathOut << endl;
	TFile *fInfo = new TFile(infoPathOut, "recreate");
	hc.writeInfo(fInfo);
	cout << "[calculateCalibrationFunctions] Writing calibration file " << calibrationPathOut << endl;
	TFile *fCalibration = new TFile(calibrationPathOut, "recreate");
	hc.writeCalibration(fCalibration);


	// ---------------- draw ----------------

	Drawer dr;
	dr.drawOverview(hc);
	dr.drawModules(modules, thrs);
}