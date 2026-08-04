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

#include "ProjectionFitter.h"
#include "Convert.h"
#include <iostream>
using std::cout;
using std::endl;



void ProjectionFitter::fit(Module& m){

	findStartAndStopBins(m);

	// loop over all projections onto the ToT axis and fit Gaussians to them
	for(int i = 0; i < nBins; i++){

		int bin = startBin + i;

		TH1D* projection = m.hEDepVsTot.ProjectionX("", bin, bin);
		
		TF1* fit = createFit(projection, bin);
		projection->Fit(fit, "rq0");
		m.maxGraph.SetPoint(i, fit->GetParameter(1), m.hEDepVsTot.GetYaxis()->GetBinCenter(bin));
		m.maxGraph.SetPointError(i, fit->GetParameter(2), 0);

		if(i == 0) positionOfFirstGaussian = fit->GetParameter(1);
		if(i == nBins-1) positionOfLastGaussian = fit->GetParameter(1);

		fit->Delete();
		projection->Delete();
	}
}



void ProjectionFitter::findStartAndStopBins(Module& m){

	startBin = -1;	// first bin for which the projection is not empty
	stopBin = -1;	// largest bin for which the projection has more than 100 entries

	for(int binY = 1; binY <= m.hEDepVsTot.GetYaxis()->GetNbins(); binY++){

		TH1D *projX = m.hEDepVsTot.ProjectionX("", binY, binY);
		
		if(projX->GetEntries() > 0. && startBin == -1){
			startBin = binY;
		}
		if(projX->GetEntries() > 100){
			stopBin = binY;
		}
	}

	// if no bins fulfilled the above criteria, simply fit all projections, which will most likely not work properly in the end
	if(startBin == -1) startBin = 1;
	if(stopBin == -1) stopBin = m.hEDepVsTot.GetYaxis()->GetNbins();

	nBins = stopBin - startBin + 1;
}



TF1* ProjectionFitter::createFit(TH1D* projection, int bin){

	
	double max = projection->GetMaximum();
	double maxPos = projection->GetBinCenter(projection->GetMaximumBin());
	double stdDev = projection->GetStdDev();
	
	TF1* fit = new TF1("fit" + Convert::toStr(bin), gauss, maxPos - 2 * stdDev, maxPos + 2 * stdDev, 3);
	fit->SetParameter(0, max);
	fit->SetParameter(1, maxPos);
	
	if(stdDev < 0.1 || stdDev > 10.){
		cout << "change stdDev from " << stdDev << " to 2." << endl;
		stdDev = 2.;
	}

	fit->SetParameter(2, stdDev);
	fit->SetParLimits(2, 0.1, 10.);

	return fit;
}



double ProjectionFitter::gauss(double *x, double *p){
	//return p[0] / TMath::Sqrt(2 * TMath::Pi() * p[2] * p[2]) * TMath::Exp( -(*x-p[1]) * (*x-p[1]) / (2*p[2]*p[2])  ) + p[3];
	return p[0] / TMath::Sqrt(2 * TMath::Pi() * p[2] * p[2]) * TMath::Exp( -(*x-p[1]) * (*x-p[1]) / (2*p[2]*p[2]));
}