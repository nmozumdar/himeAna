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

#include "tracking.h"
#include "TDiffData.h"
#include "Fit.h"
#include "ProgressIndicator.h"
#include "Helpers.h"
#include "HistogramCollection.h"
#include <iostream>
#include <array>

using std::cout;
using std::endl;
using std::array;
using std::vector;

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



void tracking(TString trb3dir, TString subdir){

	// ---------------- Input ----------------
	
	if(!subdir.EndsWith("/")) subdir += "/";
	TString subdir_without_slash(subdir);
	subdir_without_slash.Remove(subdir_without_slash.Length()-1, 1);
	TDiffData input(trb3dir + "/data/tDiff/" + subdir);
	const int nEvents = input.getNEvents();

	// ---------------- First loop over events: Fill histograms ----------------
	ProgressIndicator pi(nEvents, "[tracking] Processed events:");
	Detector hime;
	TFile *testfile = new TFile("testfile.root", "recreate");
	int testCounter = 0;
	

	for(int eventCounter = 0; eventCounter < nEvents; eventCounter++){
	//for(int eventCounter = 14150; eventCounter < 14170; eventCounter++){
	//for(int eventCounter = 0; eventCounter < 10000; eventCounter++){
	//for(int eventCounter = 8980; eventCounter < 8981; eventCounter++){
		
		pi.showProgress(eventCounter);
		input.getEvent(eventCounter);
		hime.resetTracks();

		// look up which modules have registered a hit in the current event
		// -> this defines the coordinate pairs (x,z) and (y,z), to which a linear fit is applied
		for(int i = 0; i < input.nHits; i++){
			hime.setModuleFired(input.getModuleID(i), i);
		}

		bool success = Fit::fitLoopForTracks(hime);

		// if the muon track could not be reconstructed successfully, 
		// skip the rest of the current iteration 
		// and proceed with the next event
		if(!success) continue;

		// debugging / testing
		if(testCounter < 100){
			hime.writeTracks(testfile, testCounter, eventCounter);
			testCounter++;
		}
		

		// Insert data points of position (i.e. x or y coordinate, depending on the orientation of the current module)
		// and tDiff in the TGraph* for position calibration 
		//for(int i = 0; i < input.nHits; i++){
		for(int c = 0; c < hime.hitIDs.size(); c++){
			for(int i = 0; i < hime.hitIDs[c].size(); i++){
				int hitID = hime.hitIDs[c][i];
				int moduleID = input.getModuleID(hitID);
				float tDiff = input.getTDiff(hitID);
				float pos = hime.getPos(moduleID);
				hime.addHit(moduleID, pos, tDiff);
			}
		}
	}

	// add the number of entries in the TH2F of each module to its title
	hime.addNEntriesToHistTitle();

	// fill histograms and write them to a file
	TString pathOut(trb3dir + "/data/trackingForPositionCalibration/" + subdir_without_slash + ".root");
	cout << "[tracking] Writing file " << pathOut << endl;
	TFile* fileOut = new TFile(pathOut, "recreate");

	HistogramCollection hc;
	hc.fill(hime);
	hc.write(fileOut);
	
	// write the position calibration functions to a file
	hime.writeModules(fileOut); 
}
