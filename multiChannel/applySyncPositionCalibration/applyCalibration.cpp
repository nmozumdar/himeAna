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



void applyCalibration(const char* trb3dir, const char* dir, const char* filename, const char* geometryFile, const char* thresholdsFile, const char* subdir_calibration, bool plot){

	// ---------------- Input ----------------
	TDiffData input(TString(trb3dir ) + "/data/tDiff/" + TString(dir), filename);
	int nEvents = input.getNEvents();

	// ---------------- Ouput: position calibration applied on data ----------------
	// some of the leaves are simply copied -> pass TDiffData to PosCalData
	PosCalData output(TString(trb3dir) + "/data/applyPositionCalibration/" + TString(dir) + "/" + TString(filename), input);

	// ---------------- Get position calibration functions ----------------
	CalibrationFunctions posCalFuncs(TString(trb3dir) + "/data/positionCalibrationFromTracking/" + TString(subdir_calibration) + "/calibration.root" );

	// ---------------- Get positions of all modules from the geometry file ----------------
	vector<vector<string>> csvData = CSVReader::read(TString(trb3dir) + "/data/geometry/" + TString(geometryFile), 5);
	vector<Module> modules(Constants::nModules);

	// csv data contains: entry 0 -> moduleID; entry 1 -> position x; entry 2 -> position y; entry 3 -> position z; entry 4 -> orientation (horizontal / vertical)
	for(const vector<string> &line : csvData){
		int moduleID = std::stoi(line[0]);
		modules[moduleID] = Module(moduleID, std::stof(line[1]), std::stof(line[2]), std::stof(line[3]), std::stoi(line[4]));
	}

	// ---------------- Get thresholds for all modules ----------------
	// The thresholds will NOT apply a cut on the data that are written to the output TTree object!
	// Only the data shown in the histograms of "HistogramCollection hc" are affected.
	TString thresholdsFile_tstr(thresholdsFile);
	Thresholds thrs;
	if(thresholdsFile_tstr.Length()){
		thrs = Thresholds(TString(trb3dir) + "/data/thresholds/" + TString(thresholdsFile), modules.size());
	}
	else{
		thrs = Thresholds(-1, Constants::nModules);
	}

	// ---------------- Loop over events: Apply calibration on event-by-event basis ----------------
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
		output.tmean = vector<float>(input.nHits);
		
		// loop over all hits of the current event
		for(int hit = 0; hit < input.nHits; hit++){

			int moduleID = input.getModuleID(hit);
			const Module &m = modules[moduleID];

			// Depending on the orientation of the current module, one of the coordinates gets the calibrated value,
			// the other two get randomized values
			float calibratedPos = posCalFuncs(moduleID, input.getTDiff(hit));
			float randomOffset = Constants::moduleWith * (randgen.Uniform() - 0.5);
			
			// check orientation of the current module and assign values to the position coordinates
			if(m.isHorizontal){
				output.x[hit] = -calibratedPos;
				output.y[hit] = m.y + randomOffset;
			}
			else{
				output.x[hit] = m.x + randomOffset;
				output.y[hit] = -calibratedPos;
			}
			output.z[hit] = m.z + Constants::moduleDepth * (randgen.Uniform() - 0.5);
			output.tdiff[hit] = posCalFuncs.getTDiffCorr(moduleID, input.getTDiff(hit));
			output.tmean[hit] = posCalFuncs.getTMeanCorr(moduleID, input.getTSum(hit));
		}
		
		// fill all histograms
		hc.fill(input, output, thrs);

		// fill the TTree for the current event, go to next one
		output.fill();
	}

	output.write();
	hc.write();

	if(!plot) return;
//	Drawer dr;
//	dr.drawOverview(hc);
//	for(int layer = 0; layer < hc.hPosLayer.size(); layer++) dr.drawLayer(hc, layer, modules);
}
