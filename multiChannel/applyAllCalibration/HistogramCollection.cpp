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
#include "Helpers.h"
#include "Convert.h"
#include "TVector3.h"

using std::vector;



HistogramCollection::HistogramCollection(int nLayers){

	const float maxXY = 850.;
	const float maxZ = 1000.;

	hPos = TH2F("hPos", "Positions of hits in HIME", 200, -maxXY, maxXY, 200, -maxXY, maxXY);
	hPos.GetXaxis()->SetTitle("x (mm)");
	hPos.GetYaxis()->SetTitle("y (mm)");

	hZ = TH1F("hZ", "Positions of hits in HIME", 200, -maxZ, maxZ);
	hZ.GetXaxis()->SetTitle("z (mm)");
	hZ.GetYaxis()->SetTitle("Counts");
	
	hPosLayer = vector<TH2F>(nLayers);

	for(int layer = 0; layer < nLayers; layer++){
		TString name("hPosLayer");
		name += Convert::toNdigit(layer, 2);
		TString title("Positions of hits in HIME layer ");
		title += Convert::toStr(layer);
		hPosLayer[layer] = TH2F(name, title, 200, -maxXY, maxXY, 200, -maxXY, maxXY);
		hPosLayer[layer].GetXaxis()->SetTitle("x (mm)");
		hPosLayer[layer].GetYaxis()->SetTitle("y (mm)");
	}

	htof = TH1F("htof", "Hime Time of flight", 200, -1, -1);// for now
	htofCorr = TH1F("htofCorr", "Hime ToF Z corrected", 200, -1, -1);// for now
	
	htofvModule = TH2F("htofvModule", "Time of Flight vs ModuleID", 400, -10.5, 389.5, 200, -1, -1);
	htofCorrvModule = TH2F("htofCorrvModule", "Tof Z corr vs ModuleID", 400, -10.5, 389.5, 200, -1, -1);

	htDiffvModule = TH2F("htDiffvModule", "TDiff vs ModuleID", 400, -10.5, 389.5, 600, -30, 30);

	hEvModule = TH2F("hEvModule", "Energy vs ModuleID", 400, -10.5, 389.5, 1000, -10, 100);


	htSync84 = TH2F("htSync84", "Time Module - Time Module 84", 400, -10.5, 389.5, 600, -30, 30);
	htSync60 = TH2F("htSync60", "Time Module - Time Module 60", 400, -10.5, 389.5, 600, -30, 30);

}



void HistogramCollection::fill(const TDiffData& input, const CalData& output){
	
	for(int iHit = 0; iHit < output.x.size(); iHit++){
		
		hEvModule.Fill(input.getModuleID(iHit), output.energy[iHit]);

		if(output.energy[iHit] < 4.) continue;
		
		hPos.Fill(output.x[iHit], output.y[iHit]);
		hZ.Fill(output.z[iHit]);

		int layer = Helpers::getLayer(input.getModuleID(iHit));
		hPosLayer[layer].Fill(output.x[iHit], output.y[iHit]);

		htof.Fill(output.tof[iHit]);
		//TODO:change the global to tvector3 better
		TVector3 pos;
		pos.SetXYZ(output.x[iHit], output.y[iHit], output.z[iHit]);
		htofCorr.Fill(output.tof[iHit] - pos.Mag() / 299.792458);
		
		htofvModule.Fill(input.getModuleID(iHit), output.tof[iHit]);
		htofCorrvModule.Fill(input.getModuleID(iHit), output.tof[iHit] - pos.Mag() / 299.792458);


		htDiffvModule.Fill(input.getModuleID(iHit), output.tdiff[iHit]);

		for(int iHit2 = 0; iHit2 < output.x.size(); iHit2++){

			if(input.getModuleID(iHit2) == 84)
				htSync84.Fill(input.getModuleID(iHit), output.tdiff[iHit] - output.tdiff[iHit2]);

			if(input.getModuleID(iHit2) == 60)
				htSync60.Fill(input.getModuleID(iHit), output.tdiff[iHit] - output.tdiff[iHit2]);
		}
	}
}

void HistogramCollection::write(TFile* file) const {
	file->cd();
	hPos.Write();
	hZ.Write();
	htof.Write();
	htofCorr.Write();
	htofvModule.Write();
	htofCorrvModule.Write();
	htDiffvModule.Write();
	htSync84.Write();
	htSync60.Write();
	hEvModule.Write();
	for(const TH2F& h : hPosLayer) h.Write();
}
