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

#include "fillPathLengthVsTot.h"
#include "Detector.h"
#include "PosCalData.h"
#include "Track.h"
#include "Fit.h"
#include "Helpers.h"
#include "Thresholds.h"
#include "Drawer.h"
#include "ProgressIndicator.h"
#include <vector>
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



void fillPathLengthVsTot(const char *trb3dir, const char *subdir, const char* filename, const char *geometryFile, bool plot){

	// ---------------- Input ----------------
	PosCalData input(TString(trb3dir) + "/data/applyPositionCalibration/" + TString(subdir), filename);
	const int nEvents = input.getNEvents();
	TString thrsPath(TString(trb3dir) + "/data/thresholds/" + TString(subdir) + ".csv");
	cout << "[FillPathLengthVsTot] Reading thresholds from " << thrsPath.Data() << endl;
	Thresholds thrs(thrsPath, Constants::nModules);

	// ---------------- First loop over events: Fill histograms ----------------
	ProgressIndicator pi(nEvents, "[fillPathLengthVsTot] Processed events:");
	Detector hime(trb3dir, geometryFile);
	Drawer dr;
	bool trackDrawn = false;


	for(int eventCounter = 0; eventCounter < nEvents; eventCounter++){
	
		pi.showProgress(eventCounter);
		input.getEvent(eventCounter);
		Track track;
		
		// save positions of cosmic muon hits
		// identify cosmics with a cut on ToT
		for(int hitID = 0; hitID < input.nHits; hitID++){

		//	if(Helpers::tot(input, hitID) > thrs[input.getModuleID(hitID)] - 2.){

				track.addHit(input, hitID);
		//	}
		}
		
		bool success = Fit::fitLoopForTracking(track, hime);
		
		// if the muon track could not be reconstructed successfully, 
		// skip the rest of the current iteration 
		// and proceed with the next event
		if(!success) continue;

		// this needs to be done to calculate points of intersection of a track with modules
		track.calculateInverseFits();

		// Take ony hits into account that are parts of tracks (i.e. they are assumed to come from cosmic muons)
		// -> loop over hitIDs of tracks only, not over all hits of the current event!
		for(int i = 0; i < track.hitIDs.size(); i++){
			// Fill histogram for path length inside a module vs. ToT
			Helpers::fillPathLengthVsTot(track, hime, input, track.hitIDs[i]);
			// Fill histogram for position (i.e. x or y coordinate, depending on the orientation of the current module) vs. tDiff
			Helpers::fillPosVsTDiff(track, hime, input, track.hitIDs[i]);
		}
		/*
		if(!trackDrawn){
			dr.drawTrack(track, hime, input);
			trackDrawn = true;
			cout << "\nEvent No. " << eventCounter << "\n" << endl;
		}
		*/
	}
	TString pathOut(TString(trb3dir) + "/data/trackingForEnergyCalibration/" + TString(subdir) + "/" + TString(filename));
	cout << "[fillPathLengthVsTot] Writing file " << pathOut << endl;
	hime.write(pathOut);

	if(!plot) return;
//	dr.drawPosVsTDiff(hime);
//	dr.drawEDepVsTot(hime);
}
