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
#include "Constants.h"
#include <cmath>

using std::vector;



int Helpers::getLayer(int moduleID){
	return (moduleID - (moduleID % Constants::nModulesPerLayer)) / Constants::nModulesPerLayer;
}



bool Helpers::isHorizontal(int moduleID){
	return getLayer(moduleID) & 1;
}



int Helpers::countLayers(const vector<Module>& modules){
	
	vector<int> zValues;

	for(const Module& m : modules){

		int zRound = std::roundl(m.z); // round the z position of each module to 1 mm precision
		
		if(!vectorContainsApprox(zValues, zRound)){
			zValues.push_back(zRound);
		}
	}
	
	return zValues.size();
}



bool Helpers::vectorContainsApprox(const vector<int>& vec, int x){
	for(int entry : vec) if( std::abs(entry - x) <= 1 ) return true;
	return false;
}