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

#include "TGraphErrors.h"
#include "TH2F.h"
#include "TF1.h"
#include "TFile.h"

class Module{
	public:
	Module();
	Module(int id, const TH2F* hPosVsTDiff);
	double getEffectiveVelocity() const { return posCalFunc->GetParameter(0); }
	double getPositionOffset() const { return posCalFunc->GetParameter(1); }
	double getEffectiveVelocityUnc() const { return posCalFunc->GetParError(0); }
	double getPositionOffsetUnc() const { return posCalFunc->GetParError(1); }
	void write(TFile* file) const;
	bool calibrationSuccessful;
	int id;
	TF1* posCalFunc;
	TH2F hPosVsTDiff;
	TGraphErrors maxGraph;
};

#endif