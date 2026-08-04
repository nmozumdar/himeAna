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

#include "HistogramCollection.h"
#include "Constants.h"
using std::vector;



HistogramCollection::HistogramCollection(){
	hVEff = TH1F("hVEff", "Effective velocity of light", Constants::nModules, 0, Constants::nModules);
	hOffs = TH1F("hOffs", "Position offset", Constants::nModules, 0, Constants::nModules);

	hVEff.GetXaxis()->SetTitle("Module ID");
	hOffs.GetXaxis()->SetTitle("Module ID");

	hVEff.GetYaxis()->SetTitle("Effective velocity of light (mm/ns)");
	hOffs.GetYaxis()->SetTitle("Position offset (mm)");
}



void HistogramCollection::fill(vector<Module>& modules){
	for(int id = 0; id < Constants::nModules; id++){
		if(!modules[id].calibrationSuccessful) continue;
		hVEff.SetBinContent(id+1, modules[id].getEffectiveVelocity());
		hOffs.SetBinContent(id+1, modules[id].getPositionOffset());
		hVEff.SetBinError(id+1, modules[id].getEffectiveVelocityUnc());
		hOffs.SetBinError(id+1, modules[id].getPositionOffsetUnc());
	}
}



void HistogramCollection::write(TFile* file){
	file->cd();
	hVEff.Write();
	hOffs.Write();
}