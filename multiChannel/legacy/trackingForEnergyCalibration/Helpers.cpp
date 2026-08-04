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

#include "Helpers.h"
#include "TMath.h"
#include "Geometry.h"


void Helpers::fillPathLengthVsTot(Track &t, Detector &d, const PosCalData &input, int hitID){

	Module &m = d.modules[input.getModuleID(hitID)];
	bool sameOA;
	float length = Geometry::pathLengthInModule(t, m, sameOA);
	float tot = Helpers::tot(input, hitID);
	m.setEDepVsTot(tot, 0.2 * length, sameOA);
}



void Helpers::fillPosVsTDiff(Track &t, Detector &d, const PosCalData &input, int hitID){

	int moduleID = input.getModuleID(hitID);
	bool horizontal = d.isHorizontal(moduleID);
	float z = input.getZ(hitID);
	float pos = t.fits[horizontal]->Eval(z);
	d.modules[moduleID].hPosVsTDiff->Fill(input.getTDiff(hitID), pos);
}



float Helpers::tot(float tot0, float tot1){
	return TMath::Sqrt(tot0 * tot1);
}



float Helpers::tot(const PosCalData &input, int hitID){
	return tot(input.getTot0(hitID), input.getTot1(hitID));
}