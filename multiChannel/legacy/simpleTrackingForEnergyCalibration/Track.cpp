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

#include "Track.h"
#include "TCanvas.h"
using std::vector;



Track::Track(){
	float maxDev = -1.;
	int hitWithMaxDev = -1;
}



void Track::clear(){
	while(hitGraph.GetN()){
		hitGraph.RemovePoint(hitGraph.GetN() - 1);
	}
	maxDev = -1.;
	hitWithMaxDev = -1;
	hitIDs.clear();
}



void Track::addHit(int hitID, float x_or_z, float y){
	// It's a little confusing to insert y as the x coordinate of the TGraph and vice versa,
	// but as most cosmic muons come from top, the slopes of the linear functions would be 
	// huge otherwise.
	// In order to avoid numerical imprecisions, the TGraph will therefore contain 2D position
	// coordinates with y as first component and the fit function will be x(y) with a slope
	// around 0 for most cosmic muons.
	hitGraph.SetPoint(hitGraph.GetN(), y, x_or_z);
	hitIDs.push_back(hitID);
}



void Track::fit(){
	hitGraph.Fit(muonTrack, "rq0");

	double x, y;
	allDevs = vector<float>(hitGraph.GetN());;
	maxDev = -1.;
	hitWithMaxDev = -1;

	for(int iHit = 0; iHit < hitGraph.GetN(); iHit++){

		hitGraph.GetPoint(iHit, y, x);
		float deviation = std::abs(x - muonTrack->Eval(y));
		allDevs[iHit] = deviation;
		
		if(deviation > maxDev){
			maxDev = deviation;
			hitWithMaxDev = iHit;
		}
	}
}



void Track::removeHitWithLargestDev(){
	hitGraph.RemovePoint(hitWithMaxDev);
	hitIDs.erase(hitIDs.begin() + hitWithMaxDev);
}



float Track::calculateAngleOfIncidence() {
	return TMath::ATan(1./TMath::Abs(getSlope())) * 180. / TMath::Pi();
}



void Track::draw(){
	TString name("canvas");
	TCanvas* can = new TCanvas(name, "", 900, 900);
	hitGraph.DrawClone("al*");
	muonTrack->Draw("same");
}