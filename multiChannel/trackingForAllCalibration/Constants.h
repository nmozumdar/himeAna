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
	const int nLayersPerWallHimeP = 12;
	// number of layers per detector wall
	const int nLayersPerWallHime = 4;
	// number of layers per detector wall
	const int nLayersPerWall = nLayersPerWallHime + nLayersPerWallHimeP;
	// total number of modules of the detector
	const int nModules = nModulesPerLayer * nLayersPerWall;
	// total number of channels
	const int nChTot = nModules * 2;


	// *** dimensions of modules ***
	
	// width of a module
	const float moduleWidth = 40;
	// depth of a module
	const float moduleDepth = 20;
	// Length of a module
	const float moduleLength = 1000;


	// *** dimensions of gaps ***

	// width of the gap between two modules of the same layer
	const float gapWidthHimeP = 1.;
	// width of the gap between two adjacent layers
	const float layerGapHimeP = 1;

	// width of the gap between two modules of the same layer
	const float gapWidthHime = 2.;
	// width of the gap between two adjacent layers
	const float layerGapHime = 40.;
	
	//Gap between walls
	const float wallGap = 465.;
	

	// *** dimensions of detector walls ***

	// total width (= total height) of the detector
	const float totalWidthHimeP = nModulesPerLayer * (moduleWidth + gapWidthHimeP);
	// total depth of the detector
	const float totalDepthHimeP = nLayersPerWallHimeP * (moduleDepth + layerGapHimeP);
	// half of the total width of the detector
	const float halfWidthHimeP = totalWidthHimeP / 2.;
	// half of the total depth of the detector
	const float halfDepthHimeP = totalDepthHimeP / 2.;



	// total width (= total height) of the detector
	const float totalWidthHime = nModulesPerLayer * (moduleWidth + gapWidthHime);
	// total depth of the detector
	const float totalDepthHime = nLayersPerWallHime * (moduleDepth + layerGapHime);
	// half of the total width of the detector
	const float halfWidthHime = totalWidthHime / 2.;
	// half of the total depth of the detector
	const float halfDepthHime = totalDepthHime / 2.;


	// *** dimensions of full detector ***
	// total depth of the detector
	const float totalDepth = totalDepthHime + totalDepthHimeP + wallGap;
	// half of the total depth of the detector
	const float halfDepth = totalDepth / 2.;
	


};

#endif
