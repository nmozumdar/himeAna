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

#ifndef PosCalData_h
#define PosCalData_h

#include "TDiffData.h"

class PosCalData{
	public:
	PosCalData(){}
	PosCalData(TString path, TDiffData &input);
	void fill();
	void write();
	
	std::vector<float> x;			// Position coordinate x
	std::vector<float> y;			// Position coordinate y
	std::vector<float> z;			// Position coordinate z
	std::vector<float> tdiff;			// Tdiff corrected time diffferecne between two pmts of a bar
	std::vector<float> tmean;			// Mean Calibrated time of Bar

	private:
	TFile *file;
	TTree *tree;
};

#endif
