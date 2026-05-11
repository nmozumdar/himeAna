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

#include "Fit.h"

using std::vector;

#include <iostream>
using std::cout;
using std::endl;



/*
	Fit linear functions to the data points (x,z) and (y,z).
	Find the point with the largest deviation to the fit function.
	If this deviation is too large, remove the point from the data and repeat the fit.
	If the number of remaining points is too low, no track could be reconstructed in the current event -> return false
	Otherwise, exit the loop and return true.
*/
bool Fit::fitLoopForTracks(Detector &d){
	
	// Iterate over the corrdinates x and y.
	for(int c = 0; c < 2; c++){

		bool largeDeviation = true;

		while( largeDeviation ){
			// Exit the method with 'false', if there are not enough data points. In this case, no track can be reconstructed.
			// Request at least 3 hits (*** for both of the coordinates x and y ***) that are distributed over at least 2 layers
			if(d.tracks[c]->GetN() < 3) return false;
			if(nLayers(d.modulesThatFired[c]) < 2) return false;
			// perform the fit
			fit(d.tracks[c], d.trackFits[c]);
			// find the point with the largest deviation to the data and remove it if necessary
			largeDeviation = checkDeviation(d.tracks[c], d.trackFits[c], d.modulesThatFired[c], d.hitIDs[c], d.T_sum[c]);
		}
	}

	// A track was successfully reconstructed for both the (x,z) and the (y,z) sets of data points. Return true.
	return true;
}



void Fit::fit(TGraph *track, TF1 *trackFit){
	track->Fit(trackFit, "rq0");
}



bool Fit::checkDeviation(TGraph *track, TF1 *trackFit, vector<Module*> &modulesThatFired, vector<int> &hitIDs, vector<float> &T_sum){

	double z, pos;

	// Assume that point 0 has the largest deviation to the fit function.
	// These values are updated if a point with a larger deviation is found in the for loop below
	int pointWithLargestDev = 0;
	float largestDev = calculateDeviation(pointWithLargestDev, track, trackFit);

	// Loop over all data points (x,z) or (y,z), depending on the "TGraph *track" that has been passed to this method
	for(int p = 1; p < track->GetN(); p++){

		track->GetPoint(p, z, pos);
		float currentDev = calculateDeviation(p, track, trackFit);
		
		if( currentDev > largestDev){
			pointWithLargestDev = p;
			largestDev = currentDev;
		}
	}

	if(largestDev > 1.2 * Constants::moduleWidth){

		track->RemovePoint(pointWithLargestDev);
		modulesThatFired.erase(modulesThatFired.begin() + pointWithLargestDev);
		hitIDs.erase(hitIDs.begin() + pointWithLargestDev);
		T_sum.erase(T_sum.begin() + pointWithLargestDev);
		return true;
	}

	return false;
}



// Calculate the deviation of a point to the fit function "TF1 *trackFit"
float Fit::calculateDeviation(int point, const TGraph *track, const TF1 *trackFit){

	double z, pos_point, pos_fit;

	track->GetPoint(point, z, pos_point);
	pos_fit = trackFit->Eval(z);

	return TMath::Abs(pos_point - pos_fit);
}



// Count the numer of layers in which at least one module fired
int Fit::nLayers(const vector<Module*> &modulesThatFired){
	vector<bool> layerFired(Constants::nLayersPerWall,false);
	for(const Module *m : modulesThatFired) layerFired[m->layer] = true;
	int sum = 0;
	for(int i : layerFired) sum += i;
	return sum;
}
