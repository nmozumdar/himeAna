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
#include "Detector.h"

#include <iostream>
using std::cout;
using std::endl;
using std::array;
using std::vector;

/*
	Fit linear functions to the data points (x,z) and (y,z).
	Find the point with the largest deviation to the fit function.
	If this deviation is too large, remove the point from the data and repeat the fit.
	If the number of remaining points is too low, no track could be reconstructed in the current event -> return false
	Otherwise, exit the loop and return true.
*/
bool Fit::fitLoopForTracking(Track &t, const Detector &d){

	// Iterate over the corrdinates x and y.

	bool largeDeviation = true;

	while( largeDeviation ){
		
		// Exit the method with 'false', if there are not enough data points. In this case, no track can be reconstructed.
		if(!hitsInAtLeastThreeLayers(t,d)) return false;
		//if(t.hits[0]->GetN() < 4) return false;
		// perform the fit
		fit(t);
		// find the point with the largest deviation to the data and remove it if necessary
		largeDeviation = checkDeviation(t);
	}

	// A track was successfully reconstructed for both the (x,z) and the (y,z) sets of data points. Return true.
	return true;
}



void Fit::fit(Track &t){
	for(int oa = 0; oa < 2; oa++){
		t.hits[oa]->Fit(t.fits[oa], "rq0");
	}
}



bool Fit::checkDeviation(Track &t){

	// Assume that point 0 has the largest deviation to the fit function.
	// These values are updated if a point with a larger deviation is found in the for loop below
	int hitWithLargestDev = 0;
	float largestDev = calculateDeviation(hitWithLargestDev, t);

	// Loop over all data points (x,z) or (y,z), depending on the "TGraph *track" that has been passed to this method
	for(int h = 1; h < t.moduleIDs.size(); h++){
		
		float currentDev = calculateDeviation(h, t);
		
		if( currentDev > largestDev){
			hitWithLargestDev = h;
			largestDev = currentDev;
		}
	}

	if(largestDev > 1.2 * Constants::moduleWidth){

		t.removeHit(hitWithLargestDev);
		return true;
	}

	return false;
}



// Calculate the deviation of a point to the fit function "TF1 *trackFit"
float Fit::calculateDeviation(int point, const Track &t){

	double z;
	array<float,2> coordinates_fit;		// (y,x) of fit at position z
	array<double,2> coordinates_hit;	// (y,x) of hit at position z

	for(int oa = 0; oa < 2; oa++){
		// get (y,z) and (x,z)
		t.hits[oa]->GetPoint(point, z, coordinates_hit[oa]);
		// evaluate fit at position z
		coordinates_fit[oa] = t.fits[oa]->Eval(z);
	}

	// calculate euclidean distance between fit and hit
	float sum = 0.;
	for(int oa = 0; oa < 2; oa++){
		float diff = coordinates_fit[oa] - coordinates_hit[oa];
		sum += diff * diff;
	}
	return TMath::Sqrt(sum);
}



bool Fit::hitsInAtLeastThreeLayers(Track &t, const Detector &d){

	vector<bool> layerHit(Constants::nLayersPerWall,false);
	
	for(int moduleID : t.moduleIDs){
		int layerID = d.getLayer(moduleID);
		layerHit[layerID] = true;
	}

	int sum = 0;
	for(bool lh : layerHit){
		sum += lh;
	}

	return sum >= 3;
}
