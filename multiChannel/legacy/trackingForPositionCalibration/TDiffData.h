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

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TChain.h"

class TDiffData{
	public:
	TDiffData(){}
	TDiffData(TString path);
	int getNEvents() const;
	void getEvent(long i);
	inline float getTDiff(int hit) const { return (*tDiff)[hit]; }
	inline float getTSum(int hit) const { return (*tSum)[hit]; }
	inline float getTot0(int hit) const { return (*tot0)[hit]; }
	inline float getTot1(int hit) const { return (*tot1)[hit]; }
	inline int getModuleID(int hit) const { return (*moduleID)[hit]; }

	// *** The following data is read from the TTree ***
	std::vector<float>* tDiff;				// Time difference between the rising signals of PMT 0 and 1
	std::vector<float>* tSum;				// Sum of the times of the rising signals of PMT 0 and 1
	std::vector<float>* tot0;				// Time over threshold of PMT 0
	std::vector<float>* tot1;				// Time over threshold of PMT 1
	std::vector<int>* moduleID;				// Identification number of the corresponding module
	int nHits;								// Number of hits in each event

	private:
	std::vector<TFile*> files;
	TChain *chain;
	int nEvents;
};

#endif