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
using std::vector;



void Drawer::drawOverview(const Results& results){
	TCanvas *canRes = new TCanvas("canRes", "", 900, 900);
	canRes->Divide(2,2);
	canRes->cd(1);
	results.hEnergyResolution.DrawClone("hist");
	for(int iPar = 0; iPar < results.hVecFitPar.size(); iPar++){
		canRes->cd(iPar+2);
		results.hVecFitPar[iPar].DrawClone("hist");
	}
}



void Drawer::drawModules(vector<Module>& modules){
	int nRows = 2;
	int nCols = 5;
	int nPadsPerCan = nRows * nCols;
	int padCounter = nPadsPerCan;
	int canCounter = 0;
	vector<TCanvas*> canvases;

	for(int iMod = 0; iMod < Constants::nModules; iMod++){
		if(!modules[iMod].hEDepVsTot.GetEntries()) continue;
		if(padCounter == nPadsPerCan){
			canvases.push_back(new TCanvas("can" + Convert::toStr(canCounter), "", 1400, 1000));
			canvases.back()->Divide(nCols, nRows);
			canCounter++;
			padCounter = 0;
		}
		canvases.back()->cd(padCounter+1);
		modules[iMod].hEDepVsTot.GetXaxis()->SetRangeUser(10.,30.);
		modules[iMod].hEDepVsTot.DrawClone("colz");
		modules[iMod].maxGraph.DrawClone("same l");
		modules[iMod].calibrationFunction->Draw("same");
		padCounter++;
	}

	for(TCanvas *can : canvases) can->Write();
}