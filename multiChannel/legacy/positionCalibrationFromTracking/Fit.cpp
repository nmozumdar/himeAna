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

#include "Fit.h"
#include "Constants.h"
#include <iostream>
using std::cout;
using std::endl;
using std::vector;



Fit::Fit(int minCountsPerProjection, int minProjections){
	this -> minCountsPerProjection = minCountsPerProjection;
	this -> minProjections = minProjections;
}



void Fit::perform(vector<Module>& modules){

	for(int id = 0; id < Constants::nModules; id++){
		Module& m = modules[id];

		if(!m.hPosVsTDiff.GetEntries()) continue;
		if(!findStartAndStopBins(m)){
			cout << "[Fit] Warning: No calibration for Module " << id << " due to lack of statistics" << endl;
			continue;
		}
		fitGaussians(m);
		fitCalibrationFunctions(m);
		m.calibrationSuccessful = true;
	}
}



bool Fit::findStartAndStopBins(Module& m){

	startBin = -1;	// first bin for which the projection has more than "minCountsPerProjection" entries
	stopBin = -1;	// last bin for which the projection has more than "minCountsPerProjection" entries
	int nProjectionsWithEnoughCounts = 0;	// number of projections with at least "minCountsPerProjection" entries

	for(int binY = 1; binY <= m.hPosVsTDiff.GetYaxis()->GetNbins(); binY++){

		TH1D *projX = m.hPosVsTDiff.ProjectionX("", binY, binY);
		
		if(projX->GetEntries() > minCountsPerProjection){
			if(startBin == -1) startBin = binY;
			if(binY > startBin) stopBin = binY;
			nProjectionsWithEnoughCounts++;
		}

		projX->Delete();
	}

	// if the above criteria are not fulfilled, don't perform the calibration
	if(startBin == -1 || stopBin == -1 || nProjectionsWithEnoughCounts < minProjections){
		startBin = -1;
		stopBin = -1;
		nBins = 0;
		return false;
	}

	nBins = stopBin - startBin + 1;

	return true;
}



void Fit::fitGaussians(Module& m){


	// loop over all projections onto the tDiff axis and fit Gaussians to them
	for(int i = 0; i < nBins; i++){

		int bin = startBin + i;

		TH1D* projection = m.hPosVsTDiff.ProjectionX("", bin, bin);

		if(projection->GetEntries() < minCountsPerProjection) continue;
		
		TF1* fit = new TF1("fit", "[0] * exp( - (x - [1]) * (x - [1]) / [2] / [2])", -15., 15.);
		fit->SetParameter(0, projection->GetMaximum());
		fit->SetParameter(1, projection->GetBinCenter(projection->GetMaximumBin()));
		fit->SetParameter(2, projection->GetStdDev());
		projection->Fit(fit, "rq0");

		double position = m.hPosVsTDiff.GetYaxis()->GetBinCenter(bin);
		double tDiff = fit->GetParameter(1);
		double tDiffUnc = fit->GetParameter(2);
		int nPoints = m.maxGraph.GetN();

		m.maxGraph.SetPoint(nPoints, tDiff, position);
		m.maxGraph.SetPointError(nPoints, tDiffUnc, 0.);

		projection->Delete();
		fit->Delete();
	}
}



void Fit::fitCalibrationFunctions(Module& m){
	m.posCalFunc->SetParameter(0, 150.);	// effective velocity of light in the scintillator -> set to 50 % of the speed of light in vacuum
	m.posCalFunc->SetParameter(1, 0.);		// position offset 

	// set range:
	// take the range of the data points in the TGraphErrors of each module
	// and extend it by 2 ns
	double smallDt, largeDt, tmp;
	m.maxGraph.GetPoint(0, smallDt, tmp);
	m.maxGraph.GetPoint(m.maxGraph.GetN()-1, largeDt, tmp);
	m.posCalFunc->SetRange(smallDt - 2., largeDt + 2.);

	m.maxGraph.Fit(m.posCalFunc, "rq0");
}