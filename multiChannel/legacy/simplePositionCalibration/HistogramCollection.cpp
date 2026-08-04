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

#include <iostream>
using std::cout;
using std::endl;
using std::array;
using std::to_string;



HistogramCollection::HistogramCollection(){

	hLeftEdge			= TH1F("hLeftEdge", "Left edge of the fit function of each projection", Constants::nModules, 0, Constants::nModules);
	hRightEdge			= TH1F("hRightEdge", "Right edge of the fit function of each projection", Constants::nModules, 0, Constants::nModules);
	hThr 				= TH1F("hhtr", "Thesholds", Constants::nModules, 0, Constants::nModules);
	hNCountsPerModule 	= TH1F("hNCountsPerModule", "Number of counts in each module", Constants::nModules, 0, Constants::nModules);

	hVEff = TH1F("hVEff", "Effective velocity of light", Constants::nModules, 0, Constants::nModules);
	hOffs = TH1F("hOffs", "Position offset", Constants::nModules, 0, Constants::nModules);
	hVEff.GetXaxis()->SetTitle("Module ID");
	hOffs.GetXaxis()->SetTitle("Module ID");
	hVEff.GetYaxis()->SetTitle("v_{\\mbox{eff}} \\mbox{ (mm/ns)}");
	hOffs.GetYaxis()->SetTitle("x_0 \\mbox{ (mm)}");
}



void HistogramCollection::fillHistograms(const Module& m, float threshold){

	int bin = m.getID() + 1; // the first bin in a TH1F object has number 1, but the moduleIDs start from 0
	hThr.SetBinContent(bin, threshold);
	hLeftEdge.SetBinContent(bin, m.getLeftEdge());
	hLeftEdge.SetBinError(bin, m.getLeftEdgeUnc());
	hRightEdge.SetBinContent(bin, m.getRightEdge());
	hRightEdge.SetBinError(bin, m.getRightEdgeUnc());
	hNCountsPerModule.SetBinContent(bin, m.hTotVsTDiff.GetEntries());

	hVEff.SetBinContent(bin, m.getVEff());
	hVEff.SetBinError(bin, m.getVEffUnc());
	hOffs.SetBinContent(bin, m.getOffs());
	hOffs.SetBinError(bin, m.getOffsUnc());
}



void HistogramCollection::writeInfo(TFile* f){
	f->cd();
	hLeftEdge.Write();
	hRightEdge.Write();
	hThr.Write();
	hNCountsPerModule.Write();
}



void HistogramCollection::writeCalibration(TFile* f){
	f->cd();
	hVEff.Write();
	hOffs.Write();
}