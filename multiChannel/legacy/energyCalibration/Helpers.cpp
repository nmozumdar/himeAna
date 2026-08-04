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
using std::vector;



int Helpers::getLayer(int m){
	int nModulesPerLayer = 24;
	int remainder = m % 24;
	return (m - remainder) / 24;
}



bool Helpers::isHorizontal(int w, int m){
	int l = getLayer(m);
	if(w) return ! ( l & 1 );
	return ( l & 1 );
}



bool Helpers::vectorContains(const vector<int>& v, int i){
	for(int entry : v) if(entry == i) return true;
	return false;
}