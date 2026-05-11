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

	wallDt = new TH1F("Delta_T_wall","Delta T Hime HimeP", 200, -50, 50);

	for(int c = 0; c < 2; c++){
		tracks[c] = new TGraph();
		tracks[c]->SetName(TString("track_") + coordinateNames[c]);
		tracks[c]->GetXaxis()->SetTitle("z \\mbox{ (mm)}");
		tracks[c]->GetYaxis()->SetTitle(TString(coordinateNames[c]) + " \\mbox{ (mm)}");
		trackFits[c] = new TF1(TString("trackFit_") + coordinateNames[c], "[0] + [1] * x", -600., 600);
	}

	// initialize modules (assign ID and position of the center of each module)
	
	for(int layer = 0; layer < Constants::nLayersPerWall; layer++){


		if(layer < 12)
		{
			//float z = - Constants::halfDepth + (layer + 0.5) * (Constants::moduleDepth + Constants::layerGap);
			float z = - Constants::halfDepth + Constants::totalDepthHime + Constants::wallGap + (layer + 0.5) * (Constants::moduleDepth + Constants::layerGapHimeP) - 0.5*Constants::layerGapHimeP;

			for(int mod = 0; mod < Constants::nModulesPerLayer; mod++){

				int moduleID = mod + layer * Constants::nModulesPerLayer;

				float x,y;
				if(isHorizontal(moduleID)){
					x = 0.;
					y = -Constants::halfWidthHimeP + (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidthHimeP);
				}
				else{
					x = -Constants::halfWidthHimeP + (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidthHimeP);
					y = 0.;
				}

				modules[moduleID] = Module(moduleID, layer, isHorizontal(moduleID), x, y, z);

				//cout << "[Detector] Creating module " << moduleID << " at x = " << x << ";  y = " << y << ";  z = " << z << endl;
			}
		}
		else
		{
			float z = - Constants::halfDepth + ((layer-12) + 0.5) * (Constants::moduleDepth + Constants::layerGapHime) - 0.5*Constants::layerGapHime;

			for(int mod = 0; mod < Constants::nModulesPerLayer; mod++){

				int moduleID = mod + layer * Constants::nModulesPerLayer;

				float x,y;
				if(isHorizontal(moduleID)){
					x = 0.;
					y = Constants::halfWidthHime - (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidthHime);
				}
				else{
					x = Constants::halfWidthHime - (mod + 0.5) * (Constants::moduleWidth + Constants::gapWidthHime);
					y = 0.;
				}

				modules[moduleID] = Module(moduleID, layer, isHorizontal(moduleID), x, y, z);

				//cout << "[Detector] Creating module " << moduleID << " at x = " << x << ";  y = " << y << ";  z = " << z << endl;
			}
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
	for(vector<float> &tsums : T_sum){
		tsums.clear();
	}
}



void Detector::setModuleFired(int moduleID, int hitID, float tsum){

//	if(moduleID > 287)
//		return;
	
	bool horiz = isHorizontal(moduleID);
	modulesThatFired[!horiz].push_back(&(modules[moduleID]));
	hitIDs[!horiz].push_back(hitID);
	T_sum[!horiz].push_back(tsum);
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

	if(moduleID < 288)
		return getLayer(moduleID) & 1;
	else
		return !(getLayer(moduleID) & 1);

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



void Detector::addHit(int moduleID, float pos, float tDiff, float tot, float tsum, float tot0, float tot1){

	Module &m = modules[moduleID];

	//Position Calibration
	m.hPosVsTDiff->Fill(tDiff, pos);


	//Sync Calibration
	bool horiz = isHorizontal(moduleID);

	float z_front =  - Constants::halfDepth;
	float c = 299.79; // mm/ns

	float z = m.z;
	float slope_x = trackFits[0]->GetParameter(1);
	float slope_y = trackFits[1]->GetParameter(1);
	float T_geometric = (z - z_front) * sqrt(1 + slope_x*slope_x + slope_y*slope_y) / c;
	float T_mean_corr = 0.5 * tsum;// - T_geometric;

	for(int i = 0; i < hitIDs[!horiz].size(); i++)
	{
		auto mod = modulesThatFired[!horiz][i];
		Module *modp1 = &modules[moduleID+1];
		if((m.layer == mod->layer) && (mod == modp1) && (moduleID+1)%24 != 0)
		{
			float z2 = mod->z;
			float T_geometric2 = (z2 - z_front) * sqrt(1 + slope_x*slope_x + slope_y*slope_y) / c;
			float T_mean_corr2 = 0.5 * T_sum[!horiz][i];// - T_geometric2;
			float DT = T_mean_corr2 - T_mean_corr;
			m.hDtNextBar->Fill(DT);
		}
	}

	//TODO: Does the hime+ last wall to hime transition very badly(because they are same orinetation). Fix at some point
	for(int i = 0; i < hitIDs[horiz].size(); i++)
	{
		auto mod = modulesThatFired[horiz][i];
		if(((m.layer+1) == mod->layer) && ((m.layer+1) < Constants::nLayersPerWall))
		{
			float z2 = mod->z;
			float T_geometric2 = (z2 - z_front) * sqrt(1 + slope_x*slope_x + slope_y*slope_y) / c;
			float T_mean_corr2 = 0.5 * T_sum[horiz][i];// - T_geometric2;
			float DT = T_mean_corr2 - T_mean_corr;
			for(int j = 0 ; j < Constants::nModules; j++){
				Module *modp1 = &modules[j];
				if(mod == modp1)
					m.hDtNextPlane->Fill((j)%24,DT);
			}
		}
	}

	if(m.id > 359)
	{
		//T_mean_corr -= T_geometric;
	       for(int c = 0; c < hitIDs.size(); c++){
                        for(int i = 0; i < hitIDs[c].size(); i++){
				auto mod = modulesThatFired[c][i];
				if(mod->id < 24)
				{
					float z2 = mod->z;
					float T_geometric2 = (z2 - z_front) * sqrt(1 + slope_x*slope_x + slope_y*slope_y) / c;
					float T_mean_corr2 = 0.5 * T_sum[c][i];// - T_geometric2;
					float DT = T_mean_corr2 - T_mean_corr;
		//			cout<<DT<<endl;
					wallDt->Fill(DT);
				}
                        }
                }
	}

	//Energy Calibration
	float pathLength = fabs(getPathLength(moduleID));
	m.avgToT->Fill(tot);
	m.ToT1->Fill(tot0);
	m.ToT2->Fill(tot1);
	if(pathLength == 0 || 0.2*pathLength < 4.)
		return;
	m.hEDepVsTot->Fill(tot, 0.2*pathLength);
}



float Detector::getPathLength(int moduleID)
{	

	Module &m = modules[moduleID];
	bool horiz = isHorizontal(moduleID);

	float slope_x = trackFits[1]->GetParameter(1);
	float slope_y = trackFits[0]->GetParameter(1);
	float intercept_x = trackFits[1]->GetParameter(0);
	float intercept_y = trackFits[0]->GetParameter(0);

	float norm = sqrt(slope_x*slope_x + slope_y*slope_y + 1.0);
	float dx = slope_x / norm;
	float dy = slope_y / norm;
	float dz = 1.0 / norm;

	float xMin, xMax, yMin, yMax;

	float zMin = m.z - Constants::moduleDepth / 2.;
	float zMax = m.z + Constants::moduleDepth / 2.;

	if(horiz) {
		xMin = -Constants::moduleLength / 2.;
		xMax = Constants::moduleLength / 2.;
		yMin = m.y - Constants::moduleWidth / 2.;
		yMax = m.y + Constants::moduleWidth / 2.;
	} else {
		xMin = m.x - Constants::moduleWidth / 2.;
		xMax = m.x + Constants::moduleWidth / 2.;
		yMin = -Constants::moduleLength / 2.;
		yMax =  Constants::moduleLength / 2.;
	}

	std::vector<float> tCrossings;

	if(slope_x != 0) {
		tCrossings.push_back((xMin - intercept_x) / dx);
		tCrossings.push_back((xMax - intercept_x) / dx);
	}

	if(slope_y != 0) {
		tCrossings.push_back((yMin - intercept_y) / dy);
		tCrossings.push_back((yMax - intercept_y) / dy);
	}
	if(dz != 0) {
		tCrossings.push_back((zMin - 0) / dz);
		tCrossings.push_back((zMax - 0) / dz);
	}


	std::vector<float> tInside;
	for(float t : tCrossings) {
		float x = intercept_x + dx * t;
		float y = intercept_y + dy * t;
		float z = 0 + dz * t;
		if(x >= xMin && x <= xMax &&
				y >= yMin && y <= yMax &&
				z >= zMin && z <= zMax) {
			tInside.push_back(t);
		}
	}

	if(tInside.size() < 2) return 0.; // track misses module

	float tEntry = *std::min_element(tInside.begin(), tInside.end());
	float tExit  = *std::max_element(tInside.begin(), tInside.end());
	float dt = tExit - tEntry;

	return dt; // 3D track length

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
		m.ToT1->Write();
		m.ToT2->Write();
		m.hDtNextBar->Write();
		m.hDtNextPlane->Write();
		m.hEDepVsTot->Write();
	}
	wallDt->Write();
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
