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

#include <array>
#include "TF1.h"
#include "TH2F.h"
#include "TFile.h"

class Module{
	public:
	Module(){}
	Module(int id, int layer, bool horizontal, float x, float y, float z);
	void write(TFile *file);
	// TGraph* for (dt,pos) data pairs, where pos is either x or y, depending of the orientation of the module.
	// This will be used to determine the position calibration function.
	TH2F* hPosVsTDiff;
	TH2F *hEDepVsTot_sameOA;
	TH1F* avgToT;
	TH1F* hDtNextBar;
	TH2F* hDtNextPlane;
	float x;
	float y;
	float z;
	int layer;
};

#endif
