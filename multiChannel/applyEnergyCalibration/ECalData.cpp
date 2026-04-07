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

#include "ECalData.h"
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;



ECalData::ECalData(TString path, PosCalData &input){

	// create TFile and TTree
	cout << "[ECalData] Writing file " << path.Data() << endl;
	file = new TFile(path, "recreate");
	tree = new TTree("tree", "tree");

	// *** The following data is written to the TTree ***
	// create branches
	
	tree->Branch("energy", &energy);                                              // Energy in MeVee
	tree->Branch("x", &(input.x));                                              // Position coordinate x
	tree->Branch("y", &(input.y));                                              // Position coordinate y
	tree->Branch("z", &(input.z));                                              // Position coordinate z
	tree->Branch("tDiff", &(input.tDiff));                              // Corrected Time difference between the rising signals of PMT 0 and 1
	tree->Branch("tSum", &(input.tSum));                                // Sum of the times of the rising signals of PMT 0 and 1
	tree->Branch("tot0", &(input.tot0));                                // Time over threshold of PMT 0
	tree->Branch("tot1", &(input.tot1));                                // Time over threshold of PMT 1
	tree->Branch("moduleID", &(input.moduleID));                        // Identification number of the corresponding module
	tree->Branch("nHits", &(input.nHits), "nHits/I");                   // Number of hits in each event
	tree->Branch("eventNumber", &(input.eventNumber), "eventNumber/g");
}



void ECalData::fill(){
	tree->Fill();
}



void ECalData::write(){
	file->cd();
	tree->Write();
}
