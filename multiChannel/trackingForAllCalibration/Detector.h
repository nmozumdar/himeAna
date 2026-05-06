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

#ifndef Detector_h
#define Detector_h

#include <array>
#include <vector>
#include "Constants.h"
#include "Module.h"
#include "TF1.h"
#include "TGraph.h"
#include "TFile.h"
#include "TRandom3.h"

class Detector{
	public: 
	Detector();
	void resetTracks();
	void setModuleFired(int moduleID, int hitID, float tsum);
	inline Module getModule(int moduleID) const { return modules[moduleID]; }
	float getPos(int moduleID) const;
	void addHit(int moduleID, float pos, float tDiff, float tot, float tsum);
	float getPathLength(int moduleID);
	void writeTracks(TFile *file, int testCounter, int eventCounter);
	void writeModules(TFile* file);
	void addNEntriesToHistTitle();
	TRandom3 randGen;
	
	// one TGraph* for (z,x) data pairs, another one for (z,y)
	std::array<TGraph*,2> tracks;
	std::array<TF1*,2> trackFits;
	std::array<std::vector<Module*>,2> modulesThatFired;
	std::array<std::vector<int>,2> hitIDs;
	std::array<std::vector<float>,2> T_sum;

	TH1F *wallDt;

	std::array<Module, Constants::nModules> modules;

	private:
	bool isHorizontal(int moduleID) const;
	int getLayer(int moduleID) const;
};

#endif
