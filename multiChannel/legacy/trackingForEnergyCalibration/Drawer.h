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

#ifndef Drawer_h
#define Drawer_h

#include "Track.h"
#include "Detector.h"
#include "TLine.h"
#include "Constants.h"
#include "TPad.h"
#include <array>

class Drawer{

	public:
	Drawer(){}
	void drawTrack(Track &t, const Detector &d, const PosCalData &input);
	void drawPosVsTDiff(Detector &d);
	void drawEDepVsTot(Detector &d);

	private:
	//std::array<std::array<std::array<TLine*,4>,Constants::nModules>,2> lines;
	void drawModule(const std::array<std::array<float,2>,2> &range, bool crossed, bool hit);
	std::array<TLine*,4> createLines(const std::array<std::array<float,2>,2> &range);
};

#endif