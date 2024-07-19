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

#include "LayerTrack.h"
#include "Constants.h"
#include "Convert.h"
#include "TCanvas.h"
#include "TAxis.h"
#include <iostream>
using std::cout;
using std::endl;
using std::vector;


LayerTrack::LayerTrack(){
	id = -1;
	horizontal = false;
	maxDev = -1.;
	hitWithMaxDev = -1;
}



LayerTrack::LayerTrack(int id, bool horizontal){
	
	this->id = id;
	this->horizontal = horizontal;
	this->maxDev = -1.;
	this->hitWithMaxDev = -1;
	
	float range = 1.1 * Constants::moduleLength/2.;
	TString name("fit_layer" + Convert::toNdigit(id, 2));
	muonTrack = new TF1(name, "[0] +  [1] * x", -range, range);

	// concerning the axes labels, see comment in Track::addPoint
	hitGraph.GetXaxis()->SetTitle("y (mm)");
	hitGraph.GetYaxis()->SetTitle("x (mm)");
}



float LayerTrack::calculateEnergyDeposition(){

	// projection of the muon track inside the module onto the x axis (parallel to ground)
	float deltaX;
	// projection of the muon track inside the module onto the y axis (perpendicular to ground)
	float deltaY;
	
	if(horizontal){
		// calculate deltaX from the slope of the track
		deltaX = getSlope() * Constants::moduleWidth;
		// assume that the muon traverses the full module width
		deltaY = Constants::moduleWidth;
	}
	else{
		// assume that the muon traverses the full module width
		deltaX = Constants::moduleWidth;
		// calculate deltaY from the slope of the track
		deltaY = Constants::moduleWidth / getSlope();
	}
	
	// if both deltaX and deltaY are known, one can calculate the track length inside the module,
	// assuming that it is exactly parallel to the layer
	// (i.e. the track is fully contained in the x-y plane, which is an approximation of course)
	float trackLengthInsideModule = TMath::Sqrt(deltaX * deltaX + deltaY * deltaY);
	
	return Constants::eDep_MeV_per_mm * trackLengthInsideModule;
}



void LayerTrack::fillHistograms(Module& m, std::array<float, 2>& tot){
	float combinedToT = TMath::Sqrt(tot[0] * tot[1]);
	// energy deposition in MeV
	// vs. time over threshold
	m.hEDepVsToT_yx.Fill(combinedToT, calculateEnergyDeposition());
	// angle measured relative to the ground in degree (i.e. slope 0 corresponds to 90 degree)
	// vs. time over threshold
	m.hAngVsToT_yx.Fill(combinedToT, calculateAngleOfIncidence());
}