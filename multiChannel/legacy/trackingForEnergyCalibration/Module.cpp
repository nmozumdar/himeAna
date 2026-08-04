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

#include "Module.h"
#include "TAxis.h"
#include "Constants.h"
#include "Convert.h"
#include <iostream>
using std::cout;
using std::endl;

using std::array;

Module::Module(array<float,3> center, int moduleID, int layerID, bool horizontal){

	id = moduleID;
	array<TString,2> orientationStr = {TString("vertical"), TString("horizontal")};
	
	TString title = TString("Module ") + Convert::toStr(moduleID) + 
		TString(", number ") + Convert::toStr(moduleID%Constants::nModulesPerLayer) + 
		TString(" in layer ") + Convert::toStr(layerID) + 
		TString(" (") + orientationStr[horizontal] + TString(")");

	hPosVsTDiff = new TH2F("hPosVsTDiff" + Convert::toNdigit(moduleID, 3), title, 100, -30, 30, 100, -600, 600);
	hPosVsTDiff->GetXaxis()->SetTitle("tDiff (ns)");
	hPosVsTDiff->GetYaxis()->SetTitle("Position (mm)");	
	hEDepVsTot_sameOA = new TH2F("hEDepVsTot_sameOA" + Convert::toNdigit(moduleID, 3), title, 90, 0, 30, 80, 0, 40);
	hEDepVsTot_sameOA->GetXaxis()->SetTitle("ToT (ns)");
	hEDepVsTot_sameOA->GetYaxis()->SetTitle("Energy deposition (MeV)");	
	hEDepVsTot_diffOA = new TH2F("hEDepVsTot_diffOA" + Convert::toNdigit(moduleID, 3), title, 90, 0, 30, 80, 0, 40);
	hEDepVsTot_diffOA->GetXaxis()->SetTitle("ToT (ns)");
	hEDepVsTot_diffOA->GetYaxis()->SetTitle("Energy deposition (MeV)");	
	
	this->center = center;

	// fill  "std::array<std::array<float,2>,3> moduleDimension"
	setupModuleDimensions(horizontal);

	// shift the module to the correct position 
	shift(center);
	
	// loop over axes which the surfaces are orthogonal to
	// (oa = "orthogonal axis")
	for(int oa = 0; oa < 3; oa++){

		// front and back
		for(int side = 0; side < 2; side++){
			
			surfaces[oa][side] = Surface( 
				getSurfSize(side, oa), 		// spatial extension of the surface
				oa, 						// axis which the surface is orthogonal to
				moduleDimension[oa][side]	// position of the surface on the axis oa
			);
		}
	}
}



void Module::setupModuleDimensions(bool horizontal){

	if(horizontal){
		moduleDimension[0] = {-Constants::moduleLength * 0.5, Constants::moduleLength * 0.5};
		moduleDimension[1] = {-Constants::moduleWidth * 0.5, Constants::moduleWidth * 0.5};
	}
	else{
		moduleDimension[1] = {-Constants::moduleLength * 0.5, Constants::moduleLength * 0.5};
		moduleDimension[0] = {-Constants::moduleWidth * 0.5, Constants::moduleWidth * 0.5};
} 
	moduleDimension[2] = { -Constants::moduleDepth * 0.5, Constants::moduleDepth * 0.5};
}



void Module::shift(array<float,3> center){

	// loop over coordinates x,y,z
	for(int c = 0; c < 3; c++){
		// loop over sides (back/front)
		for(int s = 0; s < 2; s++){
			moduleDimension[c][s] += center[c];
		}
	}
}



array<array<float,2>,2> Module::getSurfSize(int side, int oa) const {

	// y z plane
	if(oa == 0) return {moduleDimension[1], moduleDimension[2]};
	// x z plane
	if(oa == 1) return {moduleDimension[0], moduleDimension[2]};
	// x y plane
	return {moduleDimension[0], moduleDimension[1]};
}



void Module::setEDepVsTot(float tot, float eDep, bool sameOA){
	if(sameOA){
		hEDepVsTot_sameOA->Fill(tot, eDep);
	}
	else{
		hEDepVsTot_diffOA->Fill(tot, eDep);
	}
}



void Module::setPosVsTDiff(float tDiff, float pos){
	hPosVsTDiff->Fill(tDiff, pos);
}



void Module::write(){
	if(hEDepVsTot_sameOA){
		if(hEDepVsTot_sameOA->GetEntries()){
			hEDepVsTot_sameOA->Write();
		}
	}
	if(hEDepVsTot_diffOA){
		if(hEDepVsTot_diffOA->GetEntries()){
			hEDepVsTot_diffOA->Write();
		}
	}
	if(hPosVsTDiff){
		if(hPosVsTDiff->GetEntries()){
			hPosVsTDiff->Write();
		}
	}
}



void Module::print(){
	cout << "horizontal" << "\n";
	cout << "center = (" << center[0] << "," << center[1] << "," << center[2] << ")\n";
	for(int oa = 0; oa < 3; oa++){
		for(int side = 0; side < 2; side++){
			cout << "Surface " << oa*2+side << "\n";
			cout << "orthogonal to axis " << oa << "\n";
			cout << "position on axis = " << surfaces[oa][side].posOnAxis << " mm (side " << side << ")\n";
			cout << "range:   [" << surfaces[oa][side].range[0][0] << "," << surfaces[oa][side].range[0][1] << "],   ";
			cout << "[" << surfaces[oa][side].range[1][0] << "," << surfaces[oa][side].range[1][1] << "]\n";
		}
	}
	cout << endl;
}