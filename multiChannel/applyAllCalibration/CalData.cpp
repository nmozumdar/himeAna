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

#include "CalData.h"
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;



CalData::CalData(TString path, TDiffData &input){

	// create TFile and TTree
	cout << "[CalData] Writing file " << path.Data() << endl;
	file = new TFile(path, "recreate");
	tree = new TTree("tree", "tree");

	// *** The following data is written to the TTree ***
	// create branches for position calibration
	tree->Branch("x", &x);                                              // Position coordinate x
	tree->Branch("y", &y);                                              // Position coordinate y
	tree->Branch("z", &z);                                              // Position coordinate z
	tree->Branch("tDiff", &tdiff);                                      // Corrected Time difference between the rising signals of PMT 0 and 1
	tree->Branch("time", &time);                                      // Corrected mean Time of bar
	tree->Branch("tof", &tof);                            // synced time of flight
	tree->Branch("t0", &t0);                                // Time PMT 0
	tree->Branch("t1", &t1);                                // Time PMT 1
	// create branches for energy calibration
	tree->Branch("energy", &energy);                                    // Energy in MeVee
	// create branches for other relevant data (from TDiffData)
	tree->Branch("tSum", &(input.tSum));                                // Sum of the times of the rising signals of PMT 0 and 1
	tree->Branch("tot0", &(input.tot0));                                // Time over threshold of PMT 0
	tree->Branch("tot1", &(input.tot1));                                // Time over threshold of PMT 1
	tree->Branch("moduleID", &(input.moduleID));                        // Identification number of the corresponding module
	tree->Branch("nHits", &(input.nHits), "nHits/I");                   // Number of hits in each event
	tree->Branch("slowScaler", &(input.slowScaler), "slowScaler/g");	// Value of the fast scaler
	tree->Branch("fastScaler", &(input.fastScaler), "fastScaler/g");	// Value of the slow scaler
	tree->Branch("eventNumber", &(input.eventNumber), "eventNumber/g");	// Event number
}



void CalData::fill(){
	tree->Fill();
}



void CalData::write(){
	file->cd();
	tree->Write();
	file->Close();
}
