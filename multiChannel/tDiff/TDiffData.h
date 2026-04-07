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
 * In the fields of this class, the data is stored that will be written to a ROOT file for the further analysis, such as position and energy calibrations.
 */
#ifndef TDiffData_h
#define TDiffData_h

#include <array>
#include <vector>
#include "TFile.h"
#include "TTree.h"

class TDiffData{
	public:
	TDiffData(){}
	TDiffData(TString path);
	void fill();
	void write();
	void reset(int size = 0);
	inline TFile* getFile(){ return file; }

	// *** The following data is written to the TTree ***
	std::vector<float> tDiff;				// Time difference between the rising signals of PMT 0 and 1
	std::vector<float> t0;				// Leading edge PMT 0
	std::vector<float> t1;				// Leading edge PMT 1
	std::vector<float> tSum;				// Sum of the times of the rising signals of PMT 0 and 1
	std::vector<float> tofRaw;              // Time difference relative to a reference channel
	std::vector<float> tot0;				// Time over threshold of PMT 0
	std::vector<float> tot1;				// Time over threshold of PMT 1
	std::vector<int> moduleID;				// Identification number of the corresponding module
	int nHits;								// Number of hits in each event
	uint64_t slowScaler;                    // Value of the slow scaler 
	uint64_t fastScaler;                    // Value of the fast scaler 
	uint64_t eventNumber; 						// Event Number created by HIME DAQ

	private:
	TFile *file;
	TTree *tree;
	std::vector<std::vector<float>*> allVectors;
};

#endif
