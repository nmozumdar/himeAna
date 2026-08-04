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

using std::array;

Module::Module(){
	calibrationSuccessful = false;
}



Module::Module(int id, const TH2F* hPosVsTDiff, const TH1F* hDtNextBar, const TH2F* hDtNextPlane){
	this -> id = id;
	this -> hPosVsTDiff = TH2F(*hPosVsTDiff);
	this -> hDtNextBar = TH1F(*hDtNextBar);
	this -> hDtNextPlane = TH2F(*hDtNextPlane);
	posCalFunc = new TF1("posCalFunc_" + Convert::toNdigit(id, 3), "[0] / 2. * x + [1]", -Constants::moduleLength / 2., Constants::moduleLength / 2.);
	calibrationSuccessful = false;
}



void Module::write(TFile* file) const {
	file->cd();
	hPosVsTDiff.Write();
}
