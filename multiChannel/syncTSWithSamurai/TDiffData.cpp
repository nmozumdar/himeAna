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

#include "TDiffData.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;



TDiffData::TDiffData(TString path, TString filename){
	
	// open files		
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
	cout << "[TDiffData] " << nEvents << " events" << endl;

	// set branch addresses 
	tDiff 		= nullptr;
	tSum 		= nullptr;
	tofRaw		= nullptr;
	tot0 		= nullptr;
	tot1 		= nullptr;
	t0 		= nullptr;
	t1 		= nullptr;
	moduleID 	= nullptr;
	nHits       = -1;
	slowScaler  = -1;
	fastScaler  = -1;
	eventNumber = -1;
	tree->SetBranchAddress("tDiff", &tDiff);
	tree->SetBranchAddress("tSum", &tSum);
	tree->SetBranchAddress("tofRaw", &tofRaw);
	tree->SetBranchAddress("tot0", &tot0);
	tree->SetBranchAddress("tot1", &tot1);
	tree->SetBranchAddress("t0", &t0);
	tree->SetBranchAddress("t1", &t1);
	tree->SetBranchAddress("moduleID", &moduleID);
	tree->SetBranchAddress("nHits", &nHits);
	tree->SetBranchAddress("slowScaler", &slowScaler);
	tree->SetBranchAddress("fastScaler", &fastScaler);
	tree->SetBranchAddress("eventNumber", &eventNumber);
}



int TDiffData::getNEvents() const {
	return nEvents;
}



void TDiffData::getEvent(long i){
	tree->GetEvent(i);
}
