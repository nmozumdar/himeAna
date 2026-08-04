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

#include "TDiffFitFunction.h"
#include "TMath.h"

using std::array;



TDiffFitFunction::TDiffFitFunction(double fitRangeLow, double fitRangeUp, const char *name){

	fitParNames = {
		"gauss_scale",
		"gauss_left_width",
		"gauss_left_pos",
		"gauss_right_width",
		"gauss_left_to_right_dist"
	};

	fTF1 = new TF1( name, this, &TDiffFitFunction::evaluate, fitRangeLow, fitRangeUp, fitParNames.size(), "TDiffFitFunction", "Evaluate" );
}



double TDiffFitFunction::evaluate(double *tDiff, double *parameters){

	double tD 						= *tDiff;
	double gauss_scale 				= parameters[0];
	double gauss_left_width 		= parameters[1];
	double gauss_left_pos 			= parameters[2];
	double gauss_right_width 		= parameters[3];
	double gauss_left_to_right_dist = parameters[4];

	double gauss_right_pos = gauss_left_pos + gauss_left_to_right_dist;

	if( tD < gauss_left_pos ) return gauss(tD, gauss_scale, gauss_left_width, gauss_left_pos);

	if( tD > gauss_right_pos ) return gauss(tD, gauss_scale, gauss_right_width, gauss_right_pos);

	return gauss_scale;
}



double TDiffFitFunction::gauss(double x, double s, double w, double p){

	return s * TMath::Exp( - (x - p) * (x - p) / (w * w) );
}



double TDiffFitFunction::calculateChi2red(const TH1F *h){

	double tRel, deviation, binError;
	double rangeLow, rangeUp;
	double chiSq 			= 0.;
	int nDoF 				= 0;
	int nBins 			= h->GetXaxis()->GetNbins();
	int nFitParameters 	= fTF1->GetNumberFreeParameters();

	fTF1->GetRange(rangeLow, rangeUp);


	for(int bin = 1; bin <= nBins; bin++){

		tRel = h->GetXaxis()->GetBinCenter(bin);

		if( tRel < rangeLow ) 	continue;
		if( tRel > rangeUp ) 	break;

		deviation 	= h->GetBinContent(bin) - fTF1->Eval(tRel);
		binError 	= h->GetBinError(bin);

		chiSq += deviation * deviation / (binError * binError);
		nDoF++;
	}

	nDoF -= nFitParameters;

	fChi2red = chiSq / (1. * nDoF);

	return fChi2red;
}



array<float,2> TDiffFitFunction::getFWHM() const {

	array<array<float,2>,2> edges = getEdges();

	float fwhm = edges[1][0] - edges[0][0];
	float unc = TMath::Sqrt(edges[0][1] * edges[0][1] + edges[1][1] * edges[1][1]);

	return {fwhm, unc};
}



array<array<float,2>,2> TDiffFitFunction::getEdges() const {

	// best fit parameters
	double gauss_left_pos = fTF1->GetParameter(2);
	double gauss_left_width = fTF1->GetParameter(1);
	double gauss_right_width = fTF1->GetParameter(3);
	double gauss_left_to_right_dist = fTF1->GetParameter(4);

	// uncertainties of best fit parameters
	double gauss_left_pos_unc = fTF1->GetParError(2);
	double gauss_left_width_unc = fTF1->GetParError(1);
	double gauss_right_width_unc = fTF1->GetParError(3);
	double gauss_left_to_right_dist_unc = fTF1->GetParError(4);

	// calculate the positions of the left and the right edge, as well as their uncertainties
	float left_edge_pos = gauss_left_pos - gauss_left_width * TMath::Sqrt(TMath::Log(2.));
	float gauss_right_pos = gauss_left_pos + gauss_left_to_right_dist;
	float right_edge_pos = gauss_right_pos + gauss_right_width * TMath::Sqrt(TMath::Log(2.));

	float left_edge_pos_unc = TMath::Sqrt(gauss_left_pos_unc * gauss_left_pos_unc + gauss_left_width_unc * gauss_left_width_unc);
	float gauss_right_pos_unc = TMath::Sqrt(gauss_left_pos_unc * gauss_left_pos_unc + gauss_left_to_right_dist_unc * gauss_left_to_right_dist_unc * TMath::Log(2.));
	float right_edge_pos_unc = TMath::Sqrt(gauss_right_pos_unc * gauss_right_pos_unc + gauss_right_width_unc * gauss_right_width_unc * TMath::Log(2.));

	array<float,2> leftEdge = {left_edge_pos, left_edge_pos_unc};
	array<float,2> rightEdge = {right_edge_pos, right_edge_pos_unc};

	return {leftEdge, rightEdge};
}