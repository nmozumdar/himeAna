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
#include "Constants.h"

using std::array;

Track::Track(){
	array<const char*,2> fitNames = {"yzFit", "xzFit"};
	for(int oa = 0; oa < 2; oa++){
		hits[oa] = new TGraph();
		inverseFits[oa] = nullptr;
		fits[oa] = new TF1(fitNames[oa], "[0] + [1] * x", -Constants::halfDepth, Constants::halfDepth);
	}
}



Track::~Track(){
	for(TGraph* g : hits) g->Delete();
	for(TF1* f : fits) f->Delete();
	for(TF1* f : inverseFits) if(f) f->Delete();
}



void Track::addHit(const PosCalData &input, int iHit){
	hits[0]->SetPoint(hits[0]->GetN(), input.getZ(iHit), input.getY(iHit));
	hits[1]->SetPoint(hits[1]->GetN(), input.getZ(iHit), input.getX(iHit));
	moduleIDs.push_back(input.getModuleID(iHit));
	hitIDs.push_back(iHit);
}



void Track::removeHit(int iHit){
	for(TGraph *h : hits) h->RemovePoint(iHit);
	moduleIDs.erase(moduleIDs.begin() + iHit);
	hitIDs.erase(hitIDs.begin() + iHit);
}



void Track::clear(){
	for(TGraph *h : hits) h->Set(0);
	moduleIDs.clear();
	hitIDs.clear();
}



void Track::calculateInverseFits(){
	array<const char*,2> inverseFitNames = {"yzFitInverse", "xzFitInverse"};
	for(int oa = 0; oa < 2; oa++){
		if(fits[oa]->GetParameter(1) != 0.){
			inverseFits[oa] = createInverse(fits[oa], inverseFitNames[oa]);
		}
	}
}



TF1* Track::createInverse(const TF1 *original, const char *name){

	// get parameters from the original function
	float slope = original->GetParameter(1);
	float offset = original->GetParameter(0);
	
	// determine range for the inverse function
	double rangeMin, rangeMax;
	original->GetRange(rangeMin, rangeMax);
	float tmp0 = (rangeMin - offset) / slope;
	float tmp1 = (rangeMax - offset) / slope;
	float newRangeMin = tmp0 < tmp1 ? tmp0 : tmp1;
	float newRangeMax = tmp0 < tmp1 ? tmp1 : tmp0;
	
	// create inverse function 
	TF1 *inverse = new TF1(name, "(x - [0]) / [1]", newRangeMin, newRangeMax);
	
	// set parameters for the inverse function.
	// can be the same as for the original, because the form of the function is different
	inverse->SetParameter(0, offset);
	inverse->SetParameter(1, slope);
	return inverse;
}