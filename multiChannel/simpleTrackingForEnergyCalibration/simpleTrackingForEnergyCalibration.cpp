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

#include "simpleTrackingForEnergyCalibration.h"
#include "ProgressIndicator.h"
#include "HistogramCollection.h"
#include "PosCalData.h"
#include "Constants.h"
#include "LayerTrack.h"
#include "ZTrack.h"
#include "TCanvas.h"
#include "Helpers.h"
#include "Module.h"
#include <iostream>
#include <array>
using std::array;
using std::vector;
using std::cout;
using std::endl;



void simpleTrackingForEnergyCalibration(const char* trb3dir, const char* dir, const char* filename, int minMultiplicity, float maxDev){

	// ---------------- Input ----------------
	PosCalData input(TString(trb3dir) + "/data/applyPositionCalibration/" + TString(dir), filename);
	uint64_t nEvents = input.getNEvents();


	// ---------------- Initialize tracks and modules ----------------
	array<LayerTrack, Constants::nLayers> layerTracks;
	for(int layerID = 0; layerID < layerTracks.size(); layerID++){
		// make the layers aware of their position inside the detector wall
		// and tell them if they are horizontal or vertical
		layerTracks[layerID] = LayerTrack(layerID, Helpers::isEven(layerID));
	}

	ZTrack zTrack;

	array<Module, Constants::nModules> modules;
	for(int moduleID = 0; moduleID < modules.size(); moduleID++){
		// make the modules aware of their ordering
		modules[moduleID] = Module(moduleID);
	}


	// ---------------- Loop over events ----------------

	ProgressIndicator pi(nEvents, "Processed events:");
	HistogramCollection hc;
	int last = -1;

	for(uint64_t eventCounter = 0; eventCounter < nEvents; eventCounter++){

		zTrack.clear();
		
		pi.showProgress(eventCounter);
		input.getEvent(eventCounter);

		///////////////////////////////////////
		//                                   //
		//    PART I: TRACKS IN X-Y PLANE    //
		//                                   //
		///////////////////////////////////////

		// sort hits by layer
		for(int iHit = 0; iHit < input.nHits; iHit++){
			int moduleID = input.getModuleID(iHit);
			// get the layer that contains the module which has been hit 
			int layerID = Helpers::getLayerID(moduleID);
			// get the x- and y-position coordinates of the current hit
			layerTracks[layerID].addHit(iHit, input.getX(iHit), input.getY(iHit));
		}

		// correlate energy loss with ToT for the muon tracks in each layer
		for(LayerTrack& layerTrack: layerTracks){

			hc.fillBeforeCuts(layerTrack.getID(), layerTrack.getNHits());

			// will be set to true if a muon track is reconstructed successfully
			bool success = false;

			// required to ensure that some histograms of the HistogramCollection 
			// are only filled in the first iteration
			bool firstIteration = true;
			
			// check if the current layerTrack has enough hits
			while(layerTrack.getNHits() >= minMultiplicity){

				// fit a linear muon track through the position data points
				layerTrack.fit();

				// fill the histograms that show the data before applying any cuts
				if(firstIteration){
					hc.fillDevsFirstIteration(layerTrack.getID(), layerTrack.getDevs());
				}

				// check if the deviation of all hits to the reconstructed muon track is small enough
				if(layerTrack.getMaxDev() < maxDev){
					success = true;
					break;
				}

				// if there is a hit with large deviation, remove it from the layerTrack and repeat the fit
				layerTrack.removeHitWithLargestDev();
			}

			// skip this layer if no muon track can be reconstructed
			if(!success) continue;

			// The track was reconstructed successfully,
			// so now the energy deposition in each module
			// can be correlated with the ToT!
			// Loop over all remaining hits in this layerTrack.
			for(int hitID: layerTrack.getHitIDs()){
				int moduleID = input.getModuleID(hitID);
				array<float,2> tot = {input.getTot0(hitID), input.getTot1(hitID)};
				// fill correlation plots that allow for energy calibration
				layerTrack.fillHistograms(modules[moduleID], tot);
			}

			float angle = layerTrack.calculateAngleOfIncidence();
			// fill the histograms that show only the data from successfully reconstructed muon tracks,
			// i.e. after all cuts
			hc.fillAfterCuts(layerTrack.getID(), layerTrack.getNHits(), layerTrack.getDevs(), layerTrack.isHorizontal(), angle);
		}

		for(LayerTrack& layerTrack: layerTracks){
			layerTrack.clear();
		}

		////////////////////////////////////////
		//                                    //
		//    PART II: TRACKS IN Y-Z PLANE    //
		//                                    //
		////////////////////////////////////////

		// check if there is exactly one hit in each layer
		if(input.nHits < 3) continue;
		
		// Will be set to false if the hits of this event don't fulfill the conditions below.
		// No ZTrack will be reconstructed in this case.
		bool success = true;

		// check if all hits are located in different layers
		vector<bool> seenLayers(Constants::nLayers, false);
		for(int iHit = 0; iHit < input.nHits; iHit++){
			int moduleID = input.getModuleID(iHit);
			int layerID = Helpers::getLayerID(moduleID);
			// if a hit was found already in this layer, 
			// stop the loop over hits and go to the next event
			if(seenLayers[layerID]){
				success = false;
				break;
			}
			seenLayers[layerID] = true;
		}

		// Don't try to reconstruct the muon track,
		// if the x coordinates of any pair of hits is larger than the width of a bar.
		// This ensures that the muon track is at least nearly parallel to the y-z plane.
		/*
		for(int iHit0 = 0; iHit0 < input.nHits; iHit0++){
			for(int iHit1 = iHit0 + 1; iHit1 < input.nHits; iHit1++){
				if(TMath::Abs(input.getX(iHit0) - input.getY(iHit1)) > Constants::moduleWidth){
					success = false;
					break;
				}
			}
			if(!success) break;
		}
		*/

		if(!success) continue;

		// add all hits to the ZTrack
		for(int iHit = 0; iHit < input.nHits; iHit++){
			zTrack.addHit(iHit, input.getZ(iHit), input.getY(iHit));
		}

		// fit a linear muon track through the position data points
		zTrack.fit();

		// check if the deviation of all hits to the reconstructed muon track is small enough
		if(zTrack.getMaxDev() > maxDev) continue;

		for(int hitID: zTrack.getHitIDs()){
			int moduleID = input.getModuleID(hitID);
			array<float,2> tot = {input.getTot0(hitID), input.getTot1(hitID)};
			// fill correlation plots that allow for energy calibration
			zTrack.fillHistograms(modules[moduleID], tot);
		}
	}


	// ---------------- Ouput ----------------
	TString pathOut(TString(trb3dir) + "/data/simpleTrackingForEnergyCalibration/" + TString(dir) + "/" + TString(filename));
	cout << "[simpleTrackingForEnergyCalibration] Writing file " << pathOut.Data() << endl;
	TFile* fileOut = new TFile(pathOut, "recreate");
	hc.write(fileOut);
	for(Module& m: modules){
		m.write(fileOut);
	}
	fileOut->Close();
}
