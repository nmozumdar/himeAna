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

#ifndef Module_h
#define Module_h

#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"

class Module{
	
	public:
	Module();
	Module(int id, int ch_left_up, int ch_right_down);
	inline int getChLeftUp() const { return ch_left_up; }
	inline int getChRightDown() const { return ch_right_down; }
	inline int getID() const { return id; }
	void fillHistograms(float tDiff, float tot_left_up, float tot_right_down);
	void write(TFile* f) const;
	TH2F hTotVsTDiff;
	TH1F hTot_left_up;
	TH1F hTot_right_down;

	private:
	int id;
	int ch_left_up;
	int ch_right_down;
};

#endif