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

#ifndef Constants_h
#define Constants_h

#include <array>

namespace Constants{

	// *** numbers of channels, modules and layers ***

	// number of modules per layer
	const int nModulesPerLayer = 24;
	// number of layers per detector wall
	const int nLayersPerWall = 12;
	// total number of modules of the detector
	const int nModules = nModulesPerLayer * nLayersPerWall;
	// total number of channels
	const int nChTot = nModules * 2;


	// *** dimensions of modules ***
	
	// width of a module
	const float moduleWidth = 40;
	// depth of a module
	const float moduleDepth = 20;
	// length of a module
	const float moduleLength = 1000;


	// *** dimensions of gaps ***

	// width of the gap between two modules of the same layer
	const float gapWidth = 1.;
	// width of the gap between two adjacent layers
	const float layerGap = 1;
	

	// *** dimensions of detector walls ***

	// total width (= total height) of the detector
	const float totalWidth = nModulesPerLayer * (moduleWidth + gapWidth);
	// total depth of the detector
	const float totalDepth = nLayersPerWall * (moduleDepth + layerGap);
	// half of the total width of the detector
	const float halfWidth = totalWidth / 2.;
	// half of the total depth of the detector
	const float halfDepth = totalDepth / 2.;

};

#endif
