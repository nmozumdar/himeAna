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

namespace Constants{
	// number of layers
	const int nLayers = 3;

	// number of modules per layer
	const int nModulesPerLayer = 24;

	// number of modules
	const int nModules = nModulesPerLayer * nLayers;

	// number of PMTs
	const int nPMTs = 2 * nModules;

	// energy loss of cosmic muons in the scintillation material in MeV/mm
	const float eDep_MeV_per_mm = 0.2;

	// module length in mm
	const float moduleLength = 1000.;

	// module width in mm
	const float moduleWidth = 40.;

	// module depth in mm
	const float moduleDepth = 20.;
};

#endif