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

#ifndef Track_h
#define Track_h

#include <array>
#include <vector>
#include "TGraph.h"
#include "TF1.h"
#include "Module.h"
#include "PosCalData.h"

class Track{
	public:
	Track();
	~Track();
	void addHit(const PosCalData &input, int iHit);
	void removeHit(int iHit);
	void clear();
	void calculateInverseFits();

	// for all hits of the current event:
	// coordinate y vs. coordinate z and
	// coordinate x vs. coordinate z 
	std::array<TGraph*,2> hits;
	std::vector<int> moduleIDs;
	std::vector<int> hitIDs;

	// linear fit functions for x vs. z and y vs. z
	std::array<TF1*,2> fits;
	std::array<TF1*,2> inverseFits;

	// range of the track, i.e.
	// { {x_min, x_max}, {y_min, y_max}, {z_min, z_max} }
	// {z_min, z_max} is the same in all cases:
	// it ranges over the spatial extension of the detector wall
	std::array<std::array<float,2>,3> range;

	private:
	TF1* createInverse(const TF1 *original, const char *name);
};

#endif