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

#include "applyCalibration.h"
#include "ProgressIndicator.h"
#include "HistogramCollection.h"
#include "CalData.h"
#include "CalibrationFunctions.h"
#include "Constants.h"
#include "CSVReader.h"
#include "Helpers.h"
#include "TRandom2.h"
#include "Drawer.h"
#include <string>
#include <iostream>
using std::vector;
using std::string;
using std::cout;
using std::endl;



void applyCalibration(const char* trb3dir, const char* dir, const char* filename, const char* geometryFile, const char* subdir_Calibration, bool plot){

	//TDiffData input(TString(trb3dir ) + "/data/tDiff/" + TString(dir), filename);
	TDiffData input(TString(trb3dir ) + "/tDiff/" + TString(dir), filename);
	int nEvents = input.getNEvents();

	//CalData output(TString(trb3dir) + "/data/calibratedFiles/" + TString(dir) + "/" + TString(filename), input);
	CalData output(TString(trb3dir) + "/calibratedFiles/" + TString(dir) + "/" + TString(filename), input);

	//CalibrationFunctions CalFuncs(TString(trb3dir) + "/data/CalibrationFromTracking/" + TString(subdir_Calibration) + "/calibration.root");
	CalibrationFunctions CalFuncs(TString(trb3dir) + "/calibration/calibrationParameter/" + TString(subdir_Calibration) + "/calibration.root");

	//vector<vector<string>> csvData = CSVReader::read(TString(trb3dir) + "/data/geometry/" + TString(geometryFile), 5);
	vector<vector<string>> csvData = CSVReader::read(TString(trb3dir) + "/geometry/" + TString(geometryFile), 5);
	vector<Module> modules(Constants::nModules);

	for(const vector<string> &line : csvData){
		int moduleID = std::stoi(line[0]);
		modules[moduleID] = Module(moduleID, std::stof(line[1]), std::stof(line[2]), std::stof(line[3]), std::stoi(line[4]));
	}

	HistogramCollection hc(Helpers::countLayers(modules));
	TRandom2 randgen;
	ProgressIndicator pi(nEvents, "[calibration] Writing calibrated data. Processed events:");

	for(int eventCounter = 0; eventCounter < nEvents; eventCounter++){
		
		pi.showProgress(eventCounter);
		input.getEvent(eventCounter);

		// initialize vectors for the current event
		output.x = vector<float>(input.nHits);
		output.y = vector<float>(input.nHits);
		output.z = vector<float>(input.nHits);
		output.tdiff = vector<float>(input.nHits);
		output.time = vector<float>(input.nHits);
		output.tof = vector<float>(input.nHits);
		output.t0 = vector<float>(input.nHits);
		output.t1 = vector<float>(input.nHits);
		output.energy = vector<float>(input.nHits);
		
		// loop over all hits of the current event
		for(int hit = 0; hit < input.nHits; hit++){

			int moduleID = input.getModuleID(hit);
			const Module &m = modules[moduleID];

			float calibratedPos = CalFuncs.getPosCalibratedValue(moduleID, input.getTDiff(hit));
			float randomOffset = Constants::moduleWith * (randgen.Uniform() - 0.5);
			
			if(m.isHorizontal){
				output.x[hit] = calibratedPos;
				output.y[hit] = m.y + randomOffset;
			}
			else{
				output.x[hit] = m.x + randomOffset;
				output.y[hit] = calibratedPos;
			}
			output.z[hit] = m.z + Constants::moduleDepth * (randgen.Uniform() - 0.5);
			output.tof[hit] = CalFuncs.getTofCorr(moduleID, input.getTofRaw(hit));
			output.t0[hit] = CalFuncs.getPmtTCorr(moduleID, input.getT0(hit), input.getTot0(hit), 0);
			output.t1[hit] = CalFuncs.getPmtTCorr(moduleID, input.getT1(hit), input.getTot1(hit), 1);
			output.tdiff[hit] = output.t1[hit] - output.t0[hit];
			output.time[hit] = (output.t0[hit] + output.t1[hit]) / 2.;
			output.energy[hit] = CalFuncs.getECalibratedValue(moduleID, input.getCombinedTot(hit));
		}
		hc.fill(input, output);

		output.fill();
	}

	output.write();

	//TFile* histFile = new TFile(TString(trb3dir) + "/data/calibratedFiles/" + TString(dir) + "/hist_" + TString(filename), "recreate");
	TFile* histFile = new TFile(TString(trb3dir) + "/calibratedFiles/" + TString(dir) + "/hist_" + TString(filename), "recreate");
        hc.write(histFile);

	//cout << "[applyCalibration] Calibration Finished. File written to " << TString(trb3dir) + "/data/calibratedFiles/" + TString(dir) + "/" + TString(filename) << endl;
	cout << "[applyCalibration] Calibration Finished. File written to " << TString(trb3dir) + "/calibratedFiles/" + TString(dir) + "/" + TString(filename) << endl;
	//cout << "[applyCalibration] Histogram file written to " << TString(trb3dir) + "/data/calibratedFiles/" + TString(dir) + "/hist_" + TString(filename) << endl;
	cout << "[applyCalibration] Histogram file written to " << TString(trb3dir) + "/calibratedFiles/" + TString(dir) + "/hist_" + TString(filename) << endl;

	if(!plot) return;
//	Drawer dr;
//	dr.drawOverview(hc);
//	for(int layer = 0; layer < hc.hPosLayer.size(); layer++) dr.drawLayer(hc, layer, modules);
}
