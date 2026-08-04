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

#include "Input.h"
#include "Constants.h"
#include "Convert.h"
using std::vector;



Input::Input(TString path, vector<Module>& modules){
	
	modules = vector<Module>(Constants::nModules);
	file = new TFile(path, "read");

	for(int moduleID = 0; moduleID < Constants::nModules; moduleID++){
		TString histName("hPosVsTDiff_module_" + Convert::toNdigit(moduleID, 3));
		modules[moduleID] = Module(moduleID, (TH2F*) file->Get(histName));
	}
}