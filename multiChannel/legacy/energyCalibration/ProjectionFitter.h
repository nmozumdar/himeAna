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

#ifndef ProjectionFitter_h
#define ProjectionFitter_h

#include "Module.h"
#include "TF1.h"
#include "TH1D.h"

class ProjectionFitter{
	public:
	ProjectionFitter(){}
	void fit(Module& m);

	double getPositionOfFirstGaussian() const { return positionOfFirstGaussian; }
	double getPositionOfLastGaussian() const { return positionOfLastGaussian; }

	private:
	int nBins;
	int startBin;
	int stopBin;
	double positionOfFirstGaussian;
	double positionOfLastGaussian;
	void findStartAndStopBins(Module& m);
	TF1* createFit(TH1D* projection, int bin);
	static double gauss(double *x, double *p);
};

#endif