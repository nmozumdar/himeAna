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

#ifndef HistogramCollection_h
#define HistogramCollection_h

#include <array>
#include "TH2F.h"
#include "TH1F.h"
#include "Module.h"
#include "Constants.h"
#include "CalibrationFunction.h"
#include "TFile.h"
#include "TString.h"

class HistogramCollection{
	public:
	HistogramCollection();
	void fillHistograms(const Module& m, float threshold);
	void writeInfo(TFile* f);
	void writeCalibration(TFile* f);
	TH1F hLeftEdge;
	TH1F hRightEdge;
	TH1F hThr;
	TH1F hNCountsPerModule;

	TH1F hVEff;
	TH1F hOffs;
};

#endif