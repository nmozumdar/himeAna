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

#ifndef CalibrationFunctions_h
#define CalibrationFunctions_h

#include "TF1.h"
#include "TString.h"
#include "TFile.h"
#include <vector>

class CalibrationFunctions{
	public:
	CalibrationFunctions(){}
	CalibrationFunctions(TString path);
	// Just calls "float getCalibratedValue(int moduleID, float tDiff)"
	float operator()(int moduleID, float avgToT);
	// If there exists a calibration function for the current module, apply the calibration and return the calibrated position.
	// Otherwise, return -10000.
	float getCalibratedValue(int moduleID, float avgToT);
	private:
	TFile *file;
	std::vector<TF1*> ECalFuncs;
};

#endif
