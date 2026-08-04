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
#include "TH1F.h"
#include "TH2F.h"
#include "TStyle.h"
#include "Geometry.h"
#include "Convert.h"
#include <iostream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::array;



void Drawer::drawTrack(Track &t, const Detector &d, const PosCalData &input){

	gStyle->SetOptStat(0);

	// construct TCanvas with two TPads:
	// one pad for the x-z-plane, another one for the y-z plane
	TCanvas *can = new TCanvas("can", "", 1800, 900);
	can->Divide(2,1);

	// set up histograms that are defining the drawing range, the titles etc...
	array<TH1F*,2> dummyHistograms = {
		new TH1F("h0", "", 2, -0.55 * Constants::totalDepth, 0.55 * Constants::totalDepth),
		new TH1F("h1", "", 2, -0.55 * Constants::totalDepth, 0.55 * Constants::totalDepth)
	};
	array<TString,2> coordinateNames = {TString("y"), TString("x")};
	array<TString,2> viewDirection = {TString("right side in downstream direction"), TString("bottom")};
	array<TGraph*,2> gCopyVec;
	array<TF1*,2> fCopyVec;

	for(int oa = 0; oa < 2; oa++){

		dummyHistograms[oa]->SetTitle( coordinateNames[oa] + TString(" vs. z (view from ") + viewDirection[oa] + TString(")") );
		dummyHistograms[oa]->GetXaxis()->SetTitle("z (mm)");
		dummyHistograms[oa]->GetYaxis()->SetTitle(coordinateNames[oa] + TString(" (mm)"));
		dummyHistograms[oa]->GetYaxis()->SetRangeUser(-0.55 * Constants::moduleLength, 0.55 * Constants::moduleLength);

		can->cd(oa+1);
		gPad->SetLeftMargin(0.15);
		dummyHistograms[oa]->Draw();
		gCopyVec[oa] = new TGraph(*(t.hits[oa]));
		fCopyVec[oa] = new TF1(*(t.fits[oa]));
		gCopyVec[oa]->Draw("same *");
		fCopyVec[oa]->Draw("same");
	}

	// check which modules have been hit
	vector<bool> moduleHit(Constants::nModules, false);
	vector<bool> layerHit(Constants::nLayersPerWall, false);

	for(int iHit = 0; iHit < input.nHits; iHit++){
		int moduleID = input.getModuleID(iHit);
		moduleHit[moduleID] = true;
		int layerID = d.getLayer(moduleID);
		layerHit[layerID] = true;
	}

	// check which modules and layers the track crosses
	// -> this does not necessarily mean that these modules have registered a hit!
	vector<bool> moduleCrossed(Constants::nModules, false);
	vector<bool> layersCrossed(Constants::nLayersPerWall, false);

	for(int m = 0; m < Constants::nModules; m++){
		array<float,2> tmp;
		if(Geometry::trackCrossesModule(t, d.modules[m], tmp)){
			moduleCrossed[m] = true;
			int l = d.getLayer(m);
			layersCrossed[l] = true;
		}
	}

	// draw modules in red if they have registered a hit
	// or otherwise in black
	for(int oa = 0; oa < 2; oa++){

		can->cd(oa+1);

		for(int l = 0; l < Constants::nLayersPerWall; l++){
			
			// draw only a single module if the current layer is parallel to the plane that is drawn
			// -> in this case, only a single module is visible in the plot, because all modules lie on top of each other
			if((oa == 0 && !d.layerIsHorizontal(l)) || (oa == 1 && d.layerIsHorizontal(l))){

				int moduleID = l * Constants::nModulesPerLayer;
				array<array<float,2>,2> range = d.modules[moduleID].surfaces[oa][0].range;

				drawModule(range, layersCrossed[l], layerHit[l]);
			}
			// in the other layers, draw all modules
			else{
				for(int m = 0; m < Constants::nModulesPerLayer; m++){

					int moduleID = m + l * Constants::nModulesPerLayer;
					array<array<float,2>,2> range = d.modules[moduleID].surfaces[oa][0].range;

					drawModule(range, moduleCrossed[moduleID], moduleHit[moduleID]);
				}
			}
		}
	}
}



void Drawer::drawModule(const array<array<float,2>,2> &range, bool crossed, bool hit){
	array<TLine*,4> lines = createLines(range);
	int color = hit ? kRed : kBlack;
	int width = crossed ? 2 : 1;
	for(TLine *l : lines){
		l->SetLineWidth(width);
		l->SetLineColor(color);
		l->Draw();
	}
}



array<TLine*,4> Drawer::createLines(const array<array<float,2>,2> &range){
	float pos0 = range[0][0];
	float pos1 = range[0][1];
	float z0 = range[1][0];
	float z1 = range[1][1];

	array<TLine*,4> linesForSingleModule;
	
	// z0 -> z1
	linesForSingleModule[0] = new TLine(z0, pos0, z1, pos0);
	// pos0 -> pos1
	linesForSingleModule[1] = new TLine(z1, pos0, z1, pos1);
	// z1 -> z0
	linesForSingleModule[2] = new TLine(z1, pos1, z0, pos1);
	// pos1 -> pos0
	linesForSingleModule[3] = new TLine(z0, pos1, z0, pos0);

	return linesForSingleModule;
}



void Drawer::drawPosVsTDiff(Detector &d){

	TCanvas *can = new TCanvas("canPosVsTDiff", "", 900, 900);
	can->Divide(6,4);
	
	int pad = 1;
	
	for(int m = 0; m < d.modules.size(); m++){
		
		TH2F *hPosVsTDiff = d.modules[m].hPosVsTDiff;
		
		if(hPosVsTDiff->GetEntries()){
			can->cd(pad);
			hPosVsTDiff->Draw("colz");
			pad++;
		}
	}
}



void Drawer::drawEDepVsTot(Detector &d){

	int nRows = 4;
	int nCols = 5;
	int nPadsPerCan = nRows * nCols;
	int padCounter = nPadsPerCan;
	int canCounter = 0;
	vector<TCanvas*> canvases;

	for(int m = 0; m < d.modules.size(); m++){

		TH2F *hEDepVsTot_sameOA = d.modules[m].hEDepVsTot_sameOA;

		if(!hEDepVsTot_sameOA->GetEntries()) continue;
		
		if(padCounter == nPadsPerCan){
			canvases.push_back(new TCanvas("can" + Convert::toStr(canCounter), "", 1400, 1000));
			canvases.back()->Divide(nCols, nRows);
			canCounter++;
			padCounter = 0;
		}

		canvases.back()->cd(padCounter+1);
		hEDepVsTot_sameOA->Draw("colz");
		padCounter++;
	}
}