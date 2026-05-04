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

#include "TRB3RawData.h"

#include "Constants.h"
#include "TString.h"

#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using MF = hadaq::MessageFloat;
using HM = hadaq::HldMessage;
using SSE = hadaq::ScalerSubEvent;



TRB3RawData::TRB3RawData(TString path){

	// open file
	cout << "[TRB3RawData] Reading file " << path << endl;

	fFile = new TFile(path, "read");

	if( ! fFile ){
		cout << "[TRB3RawData] File not found" << endl;
		return;
	}

	// extract TTree from opened file
	fTree = (TTree*) fFile->Get("T");

	if( ! fTree ){
		cout << "[TRB3RawData] Tree not found" << endl;
		return;
	}

	// find number of events
	fNEvents = fTree->GetEntries();
	cout << "[TRB3RawData] " << fNEvents << " events" << endl;

	// Set branch addresses for messages of all peripheral TDCs
	fTdcNames = vector<TString>(Constants::nTdcs);
	fTdcs = vector<vector<MF>*>(Constants::nTdcs, nullptr);
	int tdcCounter = 0;

	// Loop over all branches of the tree and check if their names start with "TDC".
	// If so, set the branch address to the address of one of the vector<MF>* pointers.
	for(TObject* branch:  *fTree->GetListOfBranches()){
		TString name = branch->GetName();
		if(!name.BeginsWith("TDC")) continue;
		fTdcNames[tdcCounter] = name;
		fTree->SetBranchAddress( fTdcNames[tdcCounter], &(fTdcs[tdcCounter]) );
		cout << "[TRB3RawData] Found TDC \"" << fTdcNames[tdcCounter] << "\"" << " tdcCounter " << tdcCounter << endl;
		tdcCounter++;
		if( tdcCounter >= fTdcNames.size() ) break;
	}

	// Set branch address for run and scaler information
	runInfo = nullptr;
	scalers = nullptr;
	fTree->SetBranchAddress("HLD", &runInfo);
	fTree->SetBranchAddress("SCALER_C001", &scalers);
}



int TRB3RawData::getTrigger() const {
	return (int) runInfo->trig_type;
}



std::vector<std::vector<MF*>> TRB3RawData::getMessagesSortedByChannel(int eventNumber){
	fTree->GetEvent(eventNumber);
	
	// sort all MF objects by time
	for(vector<MF>* messages: fTdcs){
		std::sort(messages->begin(), messages->end(), [](const MF& left, const MF& right) -> bool { return left.getStamp() < right.getStamp(); });
	}

	// create a std::vector<MF*> for each channel
	std::vector<std::vector<MF*>> messagesSortedByChannel(Constants::nChTot);

	for(int iTDC = 0; iTDC < fTdcs.size(); iTDC++){
		vector<MF>* messages = fTdcs[iTDC];
		for(MF& m: *messages){
			//int ch = iTDC * Constants::nChPerTdc + (int) m.getCh() - 1;
			// cabling changed for hime+
			int ch = Constants::TDCoff[iTDC] * Constants::nChPerTdc + (int) m.getCh() - 1;
			messagesSortedByChannel[ch].push_back(&m);
		}
	}

	return messagesSortedByChannel;
}



int TRB3RawData::getNEvents() const {
	return fNEvents;
}



uint64_t TRB3RawData::getFastScaler() const {
	if(scalers) return scalers->scaler1;
	return 0.;
}



uint64_t TRB3RawData::getSlowScaler() const {
	if(scalers) return scalers->scaler2;
	return 0.;
}
