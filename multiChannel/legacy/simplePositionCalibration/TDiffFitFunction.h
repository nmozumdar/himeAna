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

#ifndef TDiffFitFunction_h
#define TDiffFitFunction_h

#include <array>
#include <vector>
#include "TF1.h"
#include "TH1F.h"

class TDiffFitFunction{

	public:

	TDiffFitFunction(){};
	TDiffFitFunction(double fitRangeLow, double fitRangeUp, const char *name);
	double evaluate(double *tDiff, double *parameters);
	int const getNParameters(){ return fitParNames.size(); }
	double calculateChi2red(const TH1F *hDiff);
	std::array<std::array<float,2>,2> getEdges() const;
	std::array<float,2> getFWHM() const;
	TF1 *fTF1;


	private:

	double fChi2red;

	double gauss(double x, double s, double w, double p);
	
	std::vector<TString> fitParNames;
};

#endif