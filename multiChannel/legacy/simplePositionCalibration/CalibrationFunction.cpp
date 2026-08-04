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

#include "CalibrationFunction.h"
#include "TAxis.h"
using std::array;




CalibrationFunction::CalibrationFunction(array<float,2> x0, array<float,2> x1, array<float,2> y0, array<float,2> y1, const char *name, const char *title){

	double dx = x1[0] - x0[0];
	double dy = y1[0] - y0[0];

	fSlope = dy / dx;
	fOffset = y0[0] - fSlope * x0[0];

	fSlopeUnc = TMath::Sqrt(
		y1[1] * y1[1] / (dx * dx)
		+ y0[1] * y0[1] / (dx * dx)
		+ x1[1] * x1[1] * dy * dy / (dx * dx * dx * dx)
		+ x0[1] * x0[1] * dy * dy / (dx * dx * dx * dx)
	);

	fOffsetUnc = TMath::Sqrt(
		y0[1] * y0[1]
		+ fSlopeUnc * fSlopeUnc * x0[0] * x0[0]
		+ fSlope * fSlope * x0[1] * x0[1]
	);

	fTF1 = new TF1("calibrationFunction", "[0] + [1] * x", x0[0], x1[0]);
	fTF1->SetParameters(fOffset, fSlope);
	
	fTF1->SetTitle("");

	setName(name);
	setTitle(title);
}



void CalibrationFunction::setName(const char *name){

	fTF1->SetName(name);
}



void CalibrationFunction::setTitle(const char *title){

	fTF1->SetTitle(title);
}



double CalibrationFunction::getCalibratedValue(double rawValue) const {

	return fOffset + fSlope * rawValue;
}