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

/*
	Iterate over all runs and add the bin contents of all TH1F and TH2F objects.
	The result is written to a ROOT file here in the sumUp directory.
*/

#include "sumUp.h"
#include "ProgressIndicator.h"
#include "LoopOverObjects.h"
#include "Input.h"
#include "TROOT.h"
#include <iostream>
using std::cout;
using std::endl;
using std::vector;

//     _______________
//     | .       ~   |
//     |  HIME ~   . |
//     | .~  rocks  .|
//     |_____________|
//            |
//      |\/\  |
//      (^.^) |
//     ,/   \-o
//  ----------------------------  

void sumUp(const char* directory, bool verbose){


	// ---------------- Input ----------------
	Input input(directory);


	// ---------------- Loop over all runs ----------------
	ProgressIndicator pi = ProgressIndicator(input.getNFiles(), "[sumUp] Added histograms: ");
	HistogramCollection hc;

	for(Int_t run = 0; run < input.getNFiles(); run++){

		if(!verbose) pi.showProgress(run);

		// get the file for the current run
		TFile* fileIn = input.getFile(run, verbose);

		// iterate over all objects in the current file
		LoopOverObjects::start(fileIn, hc, run, verbose);

		// close the file of the current run
		input.closeFile(run);
	}


	// ---------------- Output ----------------
	TFile* fileOut = new TFile("histograms.root", "recreate");
	hc.write(fileOut);


	// ---------------- Quit ----------------
	gROOT->ProcessLine(".q");
}