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
#include "Thresholds.h"
#include "Convert.h"
#include "Constants.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TLine.h"
#include <iostream>
using std::cout;
using std::endl;
using std::vector;



Drawer::Drawer(){
	gStyle->SetErrorX(0);
}



void Drawer::drawOverview(HistogramCollection& hc){
	TCanvas* can = new TCanvas("canOverview", "", 900, 900);
	can->Divide(3,2);
	can->cd(1);
	hc.hVEff.DrawClone("hist");
	hc.hVEff.DrawClone("same e1");
	can->cd(2);
	hc.hOffs.DrawClone("hist");
	hc.hOffs.DrawClone("same e1");
	can->cd(3);
	hc.hThr.DrawClone();
	can->cd(4);
	hc.hLeftEdge.DrawClone("hist");
	hc.hLeftEdge.DrawClone("same e1");
	can->cd(5);
	hc.hRightEdge.DrawClone("hist");
	hc.hRightEdge.DrawClone("same e1");
}



void Drawer::drawModules(vector<Module>& modules, const Thresholds& thrs){
	int nRows = 2;
	int nCols = 5;
	int nPadsPerCan = nRows * nCols;
	int padCounter = nPadsPerCan;
	int canCounter = 0;
	vector<TCanvas*> canvases;

	for(int id = 0; id < Constants::nModules; id++){

		if(padCounter == nPadsPerCan){
			cout << "\n[Drawer] Modules " << id << " to " << id + nPadsPerCan - 1 << ":" << endl;
			canvases.push_back(new TCanvas("can" + Convert::toStr(canCounter), "", 1400, 1000));
			canvases.back()->Divide(nCols, nRows);
			canCounter++;
			padCounter = 0;
		}

		if(modules[id].projection.GetEntries()){

			canvases.back()->cd(padCounter+1);
			gPad->Divide(1,2);

			// draw correlation plot
			canvases.back()->cd(padCounter+1)->cd(1);
			modules[id].hTotVsTDiff.DrawClone("colz");
			TLine thrLine(modules[id].hTotVsTDiff.GetXaxis()->GetXmin(), thrs[id], modules[id].hTotVsTDiff.GetXaxis()->GetXmax(), thrs[id]);
			thrLine.SetLineStyle(2);
			thrLine.SetLineColor(kRed+1);
			thrLine.DrawClone();

			// draw projection
			canvases.back()->cd(padCounter+1)->cd(2);
			modules[id].projection.DrawClone();
			modules[id].projFit->Draw("same");
		}
		padCounter++;
	}
}