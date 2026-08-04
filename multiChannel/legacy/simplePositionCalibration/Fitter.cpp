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

#include "Fitter.h"

#include "TDiffFitFunction.h"
#include "TF1.h"
#include <iostream>
using std::cout;
using std::endl;
using std::array;



void Fitter::determineEdges(Module& m, float totCut){
	array<array<float,2>,2> edges;

	const int firstYBin = m.hTotVsTDiff.GetYaxis()->FindBin(totCut);
	const int lastYBin = m.hTotVsTDiff.GetYaxis()->GetNbins();
	m.projection = *(m.hTotVsTDiff.ProjectionX(m.hTotVsTDiff.GetTitle(), firstYBin, lastYBin));

	// determine start parameters and fit range
	double mean = m.projection.GetMean();
	double width = TMath::Sqrt(12) * m.projection.GetStdDev();
	// define fit
	TDiffFitFunction *tDiffFitPtr = new TDiffFitFunction( mean-width, mean+width, "tDiffFit");
	TDiffFitFunction &tDiffFit = *tDiffFitPtr;
	m.projFit = tDiffFit.fTF1;
	// set start parameters and perform fit
	// list of parameters:
	// 0: gauss_scale
	// 1: gauss_left_width
	// 2: gauss_left_pos
	// 3: gauss_right_width
	// 4: gauss_left_to_right_dist
	tDiffFit.fTF1->SetParameters(m.projection.GetMaximum(), 0.5, mean - width / 2., 0.5, width);
	// restrict the width of the Gaussians (must be larger than the time resolution)
	tDiffFit.fTF1->SetParLimits(1, 0.08, 10.);
	tDiffFit.fTF1->SetParLimits(3, 0.08, 10.);
	// make sure that the Gaussian called "left Gaussian" is really on the left side relative to "right Gaussian"
	tDiffFit.fTF1->SetParLimits(4, 0.0, 20.);
	// perform the fit
	m.projection.Fit(tDiffFit.fTF1, "rq0");
	// print all fit parameters
	cout << "\n[Fitter] Module " << m.getID() << ":\n";
	for(int i = 0; i < 5; i++){
		cout << "Parameter " << i << ": " << tDiffFit.fTF1->GetParameter(i) << "\n";
	}
	cout << endl;
	// get result
	// { {left_edge_pos, left_edge_pos_unc}, {right_edge_pos, right_edge_pos_unc} }
	m.setEdges(tDiffFit.getEdges());
}