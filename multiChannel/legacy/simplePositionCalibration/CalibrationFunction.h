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

#ifndef CalibrationFunction_h
#define CalibrationFunction_h

#include <array>
#include "TF1.h"

class CalibrationFunction{

	public:

	CalibrationFunction(){}
	CalibrationFunction(std::array<float,2> x0, std::array<float,2> x1, std::array<float,2> y0, std::array<float,2> y1, const char *name = "", const char *title = "");
	TF1 *fTF1;
	double getSlope() const { return fSlope; }
	double getOffset() const { return fOffset; }
	double getSlopeUnc() const { return fSlopeUnc; }
	double getOffsetUnc() const { return fOffsetUnc; }
	void setName(const char *name);
	void setTitle(const char *title);
	double getCalibratedValue(double rawValue) const;

	private:

	double fSlope;
	double fOffset;
	double fSlopeUnc;
	double fOffsetUnc;
};

#endif