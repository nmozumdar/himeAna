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

#include "Geometry.h"
using std::array;

#include <iostream>
using std::cout;
using std::endl;


float Geometry::pathLengthInModule(const Track &t, const Module &m, bool &sameOA){
	array<array<float,3>,2> coordinates;
	int counter = 0;
	array<int,2> oas;

	// loop over the axes which the surfaces are orthogonal to
	for(int oa = 0; oa < 3; oa++){
		// loop over sides (there is a front surface and a back surface for each axis)
		for(int side = 0; side < 2; side++){
			
			array<float,2> coordinatesInSurface; 

			const Surface &s = m.surfaces[oa][side];
			
			if(trackCrossesSurface(t, s, coordinatesInSurface)){
				coordinates[counter] = toThreeDim(oa, s.posOnAxis, coordinatesInSurface);
				/*if(m.id == 80){
					cout << "oa = " << oa << "    side = " << side << endl;
					for(int i = 0; i < 3; i++){
						cout << coordinates[counter][i] << ", ";
					}
					cout << endl;
				}*/
				oas[counter] = oa;
				counter++;
			}
			if(counter == 2) break;
		}
	}
	if(counter < 2) return -1.;
	sameOA = (oas[0] == oas[1]);
	
	// calculate euclidean distance between coordinates[0] and coordinates[1]
	float sum = 0.;
	for(int i = 0; i < 3; i++){
		float diff = coordinates[0][i] - coordinates[1][i];
		sum += diff * diff;
	}
	return TMath::Sqrt(sum);
}



std::array<float,3> Geometry::toThreeDim(int oa, float posOnAxis, const std::array<float,2> &coordinatesInSurface){
	if(oa == 0) return {posOnAxis, coordinatesInSurface[0], coordinatesInSurface[1]};
	if(oa == 1) return {coordinatesInSurface[0], posOnAxis, coordinatesInSurface[1]};
	if(oa == 2) return {coordinatesInSurface[0], coordinatesInSurface[1], posOnAxis};
	return {0.,0.,0.};
}



bool Geometry::trackCrossesModule(const Track &t, const Module &m, array<float,2> &coordinates){
	// loop over the axes which the surfaces are orthogonal to
	for(int oa = 0; oa < 3; oa++){
		// loop over sides (there is a front surface and a back surface for each axis)
		for(int side = 0; side < 2; side++){
			if(trackCrossesSurface(t, m.surfaces[oa][side], coordinates)){
				return true;
			}
		}
	}
	return false;
}



bool Geometry::trackCrossesSurface(const Track &t, const Surface &s, array<float,2> &coordinates, bool print){
	// x y plane
	if(s.orthogonalAxis == 2){
		// get x
	 	coordinates[0] = t.fits[1]->Eval(s.posOnAxis);
		// get y
		coordinates[1] = t.fits[0]->Eval(s.posOnAxis);
	}
	// y z plane
	if(s.orthogonalAxis == 0){
		// If the track is parallel to the surface,
		// that surface can not be hit (by the present definition).
		// In this case, the inverse function "xzFitInverse" does not exist.
		// Still, a surface perpendicular to this one might be hit.
		// This is why it is important to use a closed interval below ( see (*) ),
		// not an open one.
		if(!t.inverseFits[1]) return false;
		// get z
		coordinates[1] = t.inverseFits[1]->Eval(s.posOnAxis);
		// get y
		coordinates[0] = t.fits[0]->Eval(coordinates[1]);
	}
	// x z plane
	if(s.orthogonalAxis == 1){
		// See comment with (*) above
		if(!t.inverseFits[0]) return false;
		// get z
		coordinates[1] = t.inverseFits[0]->Eval(s.posOnAxis);
		// get x
		coordinates[0] = t.fits[1]->Eval(coordinates[1]);
	}

	// (*) if coordinates[c] is an element of the *closed* interval [ s.range[c][0], s.range[c][0] ], return true
	for(int c = 0; c < 2; c++){
		if( coordinates[c] < s.range[c][0] || coordinates[c] > s.range[c][1] ) return false;
	}

	return true;

}