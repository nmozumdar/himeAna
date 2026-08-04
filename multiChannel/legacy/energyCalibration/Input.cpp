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

#include "Input.h"
#include "Constants.h"
#include "Convert.h"
#include <iostream>
#include <fstream>
using std::ifstream;
using std::string;
using std::vector;
using std::cout;
using std::endl;


Input::Input(TString path, vector<Module> &modules){

	ifstream tmpfile("tmpfile.txt~", std::ios::in);
	vector<string> filenames;
	string filename;
	while(getline(tmpfile, filename)){
		filenames.push_back(filename);
	}
	tmpfile.close();
	
	files = vector<TFile*>(filenames.size());

	if(!path.EndsWith("/")) path += "/";

	// Import histograms showing correlations of path lengths of cosmic muons and ToT
	for(int iFile = 0; iFile < filenames.size(); iFile++){

		TString pathIn = path + TString(filenames[iFile]);
		cout << "[calculateCalibrationFunctions] Reading file " << pathIn << endl;
		files[iFile] = new TFile(pathIn, "read");
		
		for(int mod = 0; mod < Constants::nModules; mod++){

			TString histName("hEDepVsTot_sameOA" + Convert::toNdigit(mod, 3));

			TH2F *h = (TH2F*) files[iFile]->Get(histName);

			if(!h) continue;

			modules[mod].addCorrelationPlot(h);
		}
		if(iFile) files[iFile]->Close();
	}
}