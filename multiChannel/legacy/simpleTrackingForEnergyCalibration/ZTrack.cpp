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

#include "ZTrack.h"
#include "Constants.h"
#include "TAxis.h"



ZTrack::ZTrack(){
	maxDev = -1.;
	hitWithMaxDev = -1;
	
	float range = 1.1 * Constants::moduleLength/2.;
	TString name("fit_z");
	muonTrack = new TF1(name, "[0] +  [1] * x", -range, range);

	// concerning the axes labels, see comment in Track::addPoint
	hitGraph.GetXaxis()->SetTitle("y (mm)");
	hitGraph.GetYaxis()->SetTitle("z (mm)");
}



float ZTrack::calculateEnergyDeposition(){

	// projection of the muon track inside the module onto the z axis (parallel to ground)
	float deltaZ = Constants::moduleDepth;
	// projection of the muon track inside the module onto the y axis (perpendicular to ground)
	float deltaY = deltaZ / getSlope();
	
	// if both deltaY and deltaZ are known, ons can calculate the track length inside the module,
	// assuming that it is exactly parallel to the y-z plane (which is an approximation of course).
	float trackLengthInsideModule = TMath::Sqrt(deltaY * deltaY + deltaZ * deltaZ);
	
	return Constants::eDep_MeV_per_mm * trackLengthInsideModule;
}



void ZTrack::fillHistograms(Module& m, std::array<float, 2>& tot){
	float combinedToT = TMath::Sqrt(tot[0] * tot[1]);
	// energy deposition in MeV
	// vs. time over threshold
	m.hEDepVsToT_yz.Fill(combinedToT, calculateEnergyDeposition());
	// angle measured relative to the ground in degree (i.e. slope 0 corresponds to 90 degree)
	// vs. time over threshold
	m.hAngVsToT_yz.Fill(combinedToT, calculateAngleOfIncidence());
}
