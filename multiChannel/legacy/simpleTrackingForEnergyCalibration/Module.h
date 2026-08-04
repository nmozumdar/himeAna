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

#ifndef Module_h
#define Module_h

#include "Convert.h"
#include "Helpers.h"
#include "TH2F.h"
#include "TF1.h"
#include "TFile.h"
#include <array>

class Module{

	public:

	Module();
	Module(int id);

	// write histograms to a file
	void write(TFile* f);

	// correlation plot: energy deposition vs. ToT for muon tracks in the y-x plane
	TH2F hEDepVsToT_yx;

	// correlation plot: energy deposition vs. ToT for muon tracks in the y-z plane
	TH2F hEDepVsToT_yz;

	// correlation plot: angle of incidence vs. ToT for muon tracks in the y-x plane
	TH2F hAngVsToT_yx;

	// correlation plot: angle of incidence vs. ToT for muon tracks in the y-z plane
	TH2F hAngVsToT_yz;

	private:

	int id;
	
	// correlation plot: angle of incidence vs. ToT
	TH2F hAngVsToT;

	// correlation plot: energy deposition vs. ToT
	TH2F hEDepVsToT;

};

#endif