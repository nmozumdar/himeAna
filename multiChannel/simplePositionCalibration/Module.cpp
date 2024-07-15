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

#include "Module.h"
#include "Convert.h"
#include "Constants.h"



void Module::add(const TH2F* hTotVsTDiff){
	if(this->hTotVsTDiff.GetEntries()){
		this->hTotVsTDiff.Add(hTotVsTDiff);
	}
	else{
		this->hTotVsTDiff = *hTotVsTDiff;
	}
}



void Module::setID(int id){
	hTotVsTDiff.SetTitle("Module " + Convert::toStr(id));
	projection.SetTitle("Module " + Convert::toStr(id));
	this->id = id;
}



void Module::setEdges(const std::array<std::array<float,2>,2>& edges){

	this -> edges = edges;

	TString calFuncName(TString("module_") + Convert::toStr(id) + TString("_posCalFunc"));
	positionCalibrationFunction = CalibrationFunction(
		getLeftEdgeWithUnc(), 
		getRightEdgeWithUnc(), 
		{Constants::moduleDimension[1], 0.}, 
		{Constants::moduleDimension[0], 0.}, 
		calFuncName
	);

	vEff = positionCalibrationFunction.getSlope() * 2.;
	vEffUnc = positionCalibrationFunction.getSlopeUnc() * 2.;
	offs = positionCalibrationFunction.getOffset();
	offsUnc = positionCalibrationFunction.getOffsetUnc();
}