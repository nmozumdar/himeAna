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

#include "Detector.h"
#include "CSVReader.h"
#include "TFile.h"

using std::array;
using std::string;
using std::vector;



Detector::Detector(TString trb3dir, TString geometryFile){

	std::vector<std::vector<string>> geoData = CSVReader::read(trb3dir + "/data/geometry/" + geometryFile, 4);

	for(int line = 0; line < geoData.size(); line++){
		// identification number of the current module
		int moduleID = std::stoi(geoData[line][0]);
		// position of the center of the module
		array<float,3> modCenter;
		// loop over coordinates x, y, z
		for(int c = 0; c < modCenter.size(); c++){
			modCenter[c] = std::stof(geoData[line][c+1]);
		}
		// create module
		modules[moduleID] = Module(modCenter, moduleID, getLayer(moduleID), isHorizontal(moduleID));
	}
}



bool Detector::layerIsHorizontal(int layerID) const {
	return layerID & 1;
}



bool Detector::isHorizontal(int moduleID) const {

	return getLayer(moduleID) & 1;
}



int Detector::getLayer(int moduleID) const {

	int remainder = moduleID % Constants::nModulesPerLayer;

	return (moduleID - remainder) / Constants::nModulesPerLayer;
}



void Detector::write(TString path){
	TFile *file = new TFile(path, "recreate");
	for(Module &m : modules) m.write();
}