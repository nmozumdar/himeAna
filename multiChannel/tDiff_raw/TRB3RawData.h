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

/*
 * This class is used to import the data that is stored in ROOT files that result from unpacking hld files with the TRB3 software package.
 */
#ifndef TRB3RawData_h
#define TRB3RawData_h

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TdcSubEvent.h"
#include "HldProcessor.h"
#include "ScalerSubEvent.h"

class TRB3RawData{
	public:
	TRB3RawData(){}
	TRB3RawData(TString path);
	int getTrigger() const;
	int getNEvents() const;
	// return the value of the 100 MHz scaler
	uint64_t getFastScaler() const;
	// return the value of the 25 MHz scaler
	uint64_t getSlowScaler() const;
	std::vector<std::vector<hadaq::MessageFloat*>> getMessagesSortedByChannel(int eventNumber);

	private:
	TFile *fFile;
	TTree *fTree;
	std::vector<TString> fTdcNames;
	std::vector<std::vector<hadaq::MessageFloat>*> fTdcs;
	hadaq::HldMessage* runInfo;
	hadaq::ScalerSubEvent* scalers;
	int fNEvents;
};

#endif