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

#include "TH2F.h"
#include "TH1D.h"
#include "TF1.h"
#include "CalibrationFunction.h"
#include <array>

class Module{

	public:

	Module(){}
	void add(const TH2F* hTotVsTDiff);
	void setEdges(const std::array<std::array<float,2>,2>& edges);
	void setID(int id);
	inline int getID() const { return id; }
	inline float getVEff() const { return vEff; }
	inline float getOffs() const { return offs; }
	inline float getVEffUnc() const { return vEffUnc; }
	inline float getOffsUnc() const { return offsUnc; }
	inline float getLeftEdge() const { return edges[0][0]; }
	inline float getRightEdge() const { return edges[1][0]; }
	inline float getLeftEdgeUnc() const { return edges[0][1]; }
	inline float getRightEdgeUnc() const { return edges[1][1]; }
	inline std::array<float,2> getLeftEdgeWithUnc() const { return edges[0]; }
	inline std::array<float,2> getRightEdgeWithUnc() const { return edges[1]; }
	
	TH2F hTotVsTDiff;
	TH1D projection;	// projection of all muon hits onto the tDiff axis of "TH2F hTotVsTDiff"
	TF1* projFit;		// fit function for "TH1D projection"

	private:

	// results of fitting "TF1* projFit" to "TH1D projection"
	// { {left_edge_pos, left_edge_pos_unc}, {right_edge_pos, right_edge_pos_unc} }
	std::array<std::array<float,2>,2> edges;

	// linear position-calibration function
	CalibrationFunction positionCalibrationFunction;

	int id;
	float vEff;	// effective velocity of light in the scintillator
	float vEffUnc; // uncertainty
	float offs; // position offset
	float offsUnc; // uncertainty
};

#endif