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

#include "Detector.h"
#include "TString.h"
#include "TAxis.h"

#include <iostream>
using std::cout;
using std::endl;

using std::array;
using std::vector;
using std::to_string;

Detector::Detector(){

	randGen = TRandom3(42);

	// initialize instances of TGraph* representing sets of hits in t
	// and the corresponding linear fit functions for track reconstruction
	array<TString,2> coordinateNames({"x", "y"});

	for(int c = 0; c < 2; c++){
		tracks[c] = new TGraph();
		tracks[c]->SetName(TString("track_") + coordinateNames[c]);
		tracks[c]->GetXaxis()->SetTitle("z \\mbox{ (mm)}");
		tracks[c]->GetYaxis()->SetTitle(TString(coordinateNames[c]) + " \\mbox{ (mm)}");
		trackFits[c] = new TF1(TString("trackFit_") + coordinateNames[c], "[0] + [1] * x", -600., 600);
	}

	// initialize modules (assign ID and position of the center of each module)
	
	for(int layer = 0; layer < Constants::nLayersPerWall; layer++){

		float z = - Constants::halfDepth + (layer + 0.5) * (Constants::moduleDepth + Constants::layerGap);

		for(int mod = 0; mod < Constants::nModulesPerLayer; mod++){
		
			int moduleID = mod + layer * Constants::nModulesPerLayer;

			float x,y;
			if(isHorizontal(moduleID)){
				x = 0.;
				y = Constants::halfWidth - (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidth);
			}
			else{
				x = Constants::halfWidth - (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidth);
				y = 0.;
			}
		
			modules[moduleID] = Module(moduleID, layer, isHorizontal(moduleID), x, y, z);

			//cout << "[Detector] Creating module " << moduleID << " at x = " << x << ";  y = " << y << ";  z = " << z << endl;
		}
	}
}



void Detector::resetTracks(){
	for(TGraph *track : tracks){
		track->Set(0);
	}
	for(vector<Module*> &m : modulesThatFired){
		m.clear();
	}
	for(vector<int> &ids : hitIDs){
		ids.clear();
	}
}



void Detector::setModuleFired(int moduleID, int hitID){
	
	bool horiz = isHorizontal(moduleID);
	modulesThatFired[!horiz].push_back(&(modules[moduleID]));
	hitIDs[!horiz].push_back(hitID);
	Module &mod = modules[moduleID];
	TGraph *track = tracks[!horiz];

	// for horizontal modules, the y coordinate of the hit is known, and vice versa
	float pos  = horiz ? mod.y : mod.x;

	// randomize x or y
	pos += (randGen.Uniform() - 0.5) * 40.;

	// randomize z
	float z = mod.z + (randGen.Uniform() - 0.5) * 20.;
	
	track->SetPoint(track->GetN(), z, pos);
}



bool Detector::isHorizontal(int moduleID) const {

	return getLayer(moduleID) & 1;
}



int Detector::getLayer(int moduleID) const {

	int remainder = moduleID % Constants::nModulesPerLayer;

	return (moduleID - remainder) / Constants::nModulesPerLayer;
}



/*
	Evaluate the fit function for the (x,z) or (y,z) data points (depending on the orientation of the current module)
	at position z
*/
float Detector::getPos(int moduleID) const {

	float z = modules[moduleID].z;

	// trackFits[0] contains x(z) 
	// trackFits[1] contains y(z)
	return trackFits[isHorizontal(moduleID)]->Eval(z);
}



void Detector::addHit(int moduleID, float pos, float tDiff, float tot){

	Module &m = modules[moduleID];

	m.hPosVsTDiff->Fill(tDiff, pos);
	m.avgToT->Fill(tot);
}



void Detector::writeTracks(TFile *file, int testCounter, int eventCounter){
	
	file->cd();

	array<TString,2> coordinateNames({"x", "y"});
	
	for(int c = 0; c < 2; c++){
		tracks[c]->SetTitle(to_string(eventCounter).data());
		tracks[c]->Write(TString("track_") + coordinateNames[c] + TString("_") + to_string(testCounter));
		trackFits[c]->Write(TString("trackFit_") + coordinateNames[c] + TString("_") + to_string(testCounter));
	}
}



void Detector::writeModules(TFile* file){
	file->cd();
	for(Module &m : modules){
		m.hPosVsTDiff->Write();
		m.avgToT->Write();
	}
}



void Detector::addNEntriesToHistTitle(){
	for(Module &m : modules){
		TString newTitle(m.hPosVsTDiff->GetTitle());
		newTitle += ", ";
		newTitle += to_string(m.hPosVsTDiff->GetEntries());
		newTitle += " events";
		m.hPosVsTDiff->SetTitle(newTitle);
	}
}
