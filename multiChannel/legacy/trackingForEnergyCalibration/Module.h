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

#include <array>
#include "Surface.h"
#include "TH2F.h"

class Module{
	public:
	Module(){}
	Module(std::array<float,3> center, int moduleID, int layerID, bool horizontal);
	void setEDepVsTot(float tot, float eDep, bool sameOA);
	void setPosVsTDiff(float tDiff, float pos);
	void write();
	void print();
	
	std::array<float,3> center;
	std::array<std::array<Surface,2>,3> surfaces;
	TH2F *hPosVsTDiff;
	TH2F *hEDepVsTot_sameOA;
	TH2F *hEDepVsTot_diffOA;
	int id;

	private:
	std::array<std::array<float,2>,2> getSurfSize(int side, int oa) const;
	void setupModuleDimensions(bool horizontal);
	void shift(std::array<float,3> center);
	std::array<std::array<float,2>,3> moduleDimension;
};

#endif