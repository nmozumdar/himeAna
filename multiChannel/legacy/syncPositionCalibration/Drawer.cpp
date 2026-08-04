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

#include "Drawer.h"
#include "TCanvas.h"
#include "Convert.h"
#include "Constants.h"
#include "TStyle.h"

using std::vector;



void Drawer::drawOverview(HistogramCollection& hc){

	gStyle->SetErrorX(0.);
	
	TCanvas* can = new TCanvas("can", "", 1200, 600);
	can->Divide(2,1);
	
	can->cd(1);
	gPad->SetTicks();
	gPad->SetLeftMargin(0.2);
	hc.hVEff.DrawClone("e1");
	TH1F hVEffCopy(hc.hVEff);			// workaround to see error bars *and* the histogram line in the web-browser panel (combining the options hist and e1 doesn't work there yet)
	hVEffCopy.DrawClone("same hist");

	can->cd(2);
	gPad->SetTicks();
	gPad->SetLeftMargin(0.2);
	hc.hOffs.DrawClone("e1");
	TH1F hOffsCopy(hc.hOffs);
	hOffsCopy.DrawClone("same hist");
}



void Drawer::drawModules(vector<Module>& modules){
	int nRows = 2;
	int nCols = 5;
	int nPadsPerCan = nRows * nCols;
	int padCounter = nPadsPerCan;
	int canCounter = 0;
	vector<TCanvas*> canvases;

	for(int id = 0; id < Constants::nModules; id++){

		if(!modules[id].calibrationSuccessful) continue;
		
		if(padCounter == nPadsPerCan){
			canvases.push_back(new TCanvas("can" + Convert::toStr(canCounter), "", 1400, 1000));
			canvases.back()->Divide(nCols, nRows);
			canCounter++;
			padCounter = 0;
		}

		canvases.back()->cd(padCounter+1);
		modules[id].hPosVsTDiff.DrawClone("colz");
		modules[id].maxGraph.DrawClone("same");
		modules[id].posCalFunc->Draw("same");
		padCounter++;
	}
}