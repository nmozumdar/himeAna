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

#ifndef TDiffData_h
#define TDiffData_h

#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "Constants.h"
#include <vector>

class TDiffData{
	public:
	TDiffData(){}
	TDiffData(TString pathIn, TString filename);
	int getNEvents() const;
	void getEvent(long i);
	inline float getTDiff(int hit) const { return (*tDiff)[hit]; }
	inline float getTSum(int hit) const { return (*tSum)[hit]; }
	inline float getTot0(int hit) const { return (*tot0)[hit]; }
	inline float getTot1(int hit) const { return (*tot1)[hit]; }
	inline float getTofRaw(int hit) const { return (*tofRaw)[hit]; }
	inline float getT0(int hit) const { return (*t0)[hit]; }
	inline float getT1(int hit) const { return (*t1)[hit]; }
	inline float getCombinedTot(int hit) const { return std::sqrt((*tot0)[hit] * (*tot1)[hit]); }
	inline int getModuleID(int hit) const { return (*moduleID)[hit]; }

	// *** The following data are read from the TChain ***
	std::vector<float>* tDiff;              // Time difference between the rising signals of PMT 0 and 1
	std::vector<float>* tSum;               // Sum of the times of the rising signals of PMT 0 and 1
	std::vector<float>* tofRaw;             // Uncalibrated time of flight
	std::vector<float>* tot0;               // Time over threshold of PMT 0
	std::vector<float>* tot1;               // Time over threshold of PMT 1
	std::vector<int>* moduleID;             // Identification number of the corresponding module
	std::vector<float>* t0;                          // Leading edge PMT 0
	std::vector<float>* t1;                          // Leading edge PMT 1
	int nHits;                              // Number of hits in each event
	uint64_t slowScaler;                    // Value of the fast scaler
	uint64_t fastScaler;                    // Value of the slow scaler
	uint64_t eventNumber;                   // Event number
	
	private:
	TFile *file;
	TTree *tree;
	int nEvents;
};

#endif
