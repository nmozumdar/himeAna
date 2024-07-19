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

#include "PosCalData.h"

#include <iostream>

using std::cout;
using std::endl;



PosCalData::PosCalData(TString path, TString filename){

	// open file		
	TString completePath = path + TString("/") + filename;
	cout << "[TDiffData] Reading file " << completePath.Data() << endl;
	file = new TFile(completePath, "read");

	if( ! file ){
		cout << "file not found" << endl;
		return;
	}

	// extract TTree from opened file
	tree = (TTree*) file->Get("tree");

	if( ! tree ){
		cout << "tree not found" << endl;
		return;
	}
	

	// find number of events
	nEvents = tree->GetEntries();
	cout << "[PosCalData] " << nEvents << " events" << endl;

	// set branch addresses 
	x 			= nullptr;
	y 			= nullptr;
	z 			= nullptr;
	tDiff		= nullptr;
	tSum 		= nullptr;
	tot0 		= nullptr;
	tot1 		= nullptr;
	moduleID 	= nullptr;
	tree->SetBranchAddress("x", &x);
	tree->SetBranchAddress("y", &y);
	tree->SetBranchAddress("z", &z);
	tree->SetBranchAddress("tDiff", &tDiff);
	tree->SetBranchAddress("tSum", &tSum);
	tree->SetBranchAddress("tot0", &tot0);
	tree->SetBranchAddress("tot1", &tot1);
	tree->SetBranchAddress("moduleID", &moduleID);
	tree->SetBranchAddress("nHits", &nHits);
}



int PosCalData::getNEvents() const {
	return nEvents;
}



void PosCalData::getEvent(long i){
	tree->GetEvent(i);
}