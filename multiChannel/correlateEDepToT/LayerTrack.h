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

#ifndef LayerTrack_h
#define LayerTrack_h

#include "Track.h"
#include "TGraph.h"
#include "TF1.h"
#include "Module.h"
#include <vector>

class LayerTrack : public Track{
	
	public:
	LayerTrack();
	LayerTrack(int id, bool horizontal);

	// calculate the energy loss of a cosmic muon inside a module, 
	// assuming that the track is fully contained in the x-y plane 
	// and that it doesn't enter or leave the module through the smallest of its surfaces
	// (y-z plane for horizontal modules, x-z plane for vertical ones)
	float calculateEnergyDeposition();

	// get the layer ID
	inline int getID() const { return id; }

	// check if the layer is horizontal
	inline bool isHorizontal() const { return horizontal; }

	// fill the histograms that serve for the energy calibration of a module
	void fillHistograms(Module& m, std::array<float, 2>& tot);

	private:
	
	int id;
	bool horizontal;
};

#endif