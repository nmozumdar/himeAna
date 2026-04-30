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

using std::vector;



HistogramCollection::HistogramCollection(int nLayers){

	const float maxXY = 850.;
	const float maxZ = 200.;

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
}



void HistogramCollection::fill(const TDiffData& input, const PosCalData& output, const Thresholds& thrs){
	
	for(int iHit = 0; iHit < output.x.size(); iHit++){
		
		if(input.getCombinedTot(iHit) < thrs[input.getModuleID(iHit)]) continue;
		
		hPos.Fill(output.x[iHit], output.y[iHit]);
		hZ.Fill(output.z[iHit]);

		int layer = Helpers::getLayer(input.getModuleID(iHit));
		hPosLayer[layer].Fill(output.x[iHit], output.y[iHit]);
	}
}



void HistogramCollection::write() const {
	hPos.Write();
	hZ.Write();
	for(const TH2F& h : hPosLayer) h.Write();
}