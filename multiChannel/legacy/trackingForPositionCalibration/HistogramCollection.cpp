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



HistogramCollection::HistogramCollection(){
	hNTracksPerModule = new TH1F("hNTracksPerModule", "", Constants::nModules, 0, Constants::nModules);
	hNTracksPerModule->GetXaxis()->SetTitle("Module ID");
	hNTracksPerModule->GetYaxis()->SetTitle("Number of reconstructed tracks");
}




void HistogramCollection::fill(const Detector &d){
	for(int m = 0; m < Constants::nModules; m++){
		hNTracksPerModule->SetBinContent(m+1, d.modules[m].hPosVsTDiff->GetEntries());
	}
}



void HistogramCollection::write(TFile* f){
	f->cd();
	hNTracksPerModule->Write();
}