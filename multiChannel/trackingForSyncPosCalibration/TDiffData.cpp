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
#include <fstream>
using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::vector;



TDiffData::TDiffData(TString path){

	// open files
	chain = new TChain("tree", "tree");

	if(!path.EndsWith("/")) path += "/";

	ifstream tmpfile("tmpfile.txt~", std::ios::in);
	vector<string> filenames;
	string filename;
		
	while(getline(tmpfile, filename)){
		TString completePath = path + filename;
		cout << "[TDiffData] Reading file " << completePath.Data() << endl;
		chain->Add(completePath);
	}

	tmpfile.close();
	

	// find number of events
	nEvents = chain->GetEntries();
	cout << "[TDiffData] " << nEvents << " events" << endl;

	// set branch addresses 
	tDiff 		= nullptr;
	tSum 		= nullptr;
	tot0 		= nullptr;
	tot1 		= nullptr;
	moduleID 	= nullptr;
	chain->SetBranchAddress("tDiff", &tDiff);
	chain->SetBranchAddress("tSum", &tSum);
	chain->SetBranchAddress("tot0", &tot0);
	chain->SetBranchAddress("tot1", &tot1);
	chain->SetBranchAddress("moduleID", &moduleID);
	chain->SetBranchAddress("nHits", &nHits);
}



int TDiffData::getNEvents() const {
	return nEvents;
}



void TDiffData::getEvent(long i){
	chain->GetEvent(i);
}