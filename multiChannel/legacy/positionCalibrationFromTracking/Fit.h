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

#ifndef Fit_h
#define Fit_h

#include "Module.h"
#include "TH1D.h"
#include <vector>

class Fit{
	public:
	Fit(){}
	Fit(int minCountsPerProjection, int minProjections);
	void perform(std::vector<Module>& modules);
	private:
	bool findStartAndStopBins(Module& m);
	void fitGaussians(Module& m);
	void fitCalibrationFunctions(Module& m);
	int minCountsPerProjection;
	int minProjections;
	int startBin;
	int stopBin;
	int nBins;
};

#endif