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

#include "TDiffData.h"
#include "TFile.h"
#include "Convert.h"
#include <iostream>
#include <fstream>
using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::array;



TDiffData::TDiffData(TString trb3dir, TString subdir){

	ifstream tmpfile(subdir + ".txt~", std::ios::in);
	vector<string> filenames;
	string filename;
		
	while(getline(tmpfile, filename)){
		filenames.push_back(filename);		
	}

	TString path = trb3dir + "/data/tDiff/" + subdir;
	if(!path.EndsWith("/")) path += "/";

	paths = vector<TString>(filenames.size());

	for(int i = 0; i < filenames.size(); i++){
		
		paths[i] = path + filenames[i];
	}
}



void TDiffData::fillModules(vector<Module>& modules, int iVoltage) const {

	for(int iFile = 0; iFile < paths.size(); iFile++){
		
		cout << "[TDiffData] Reading file " << paths[iFile].Data() << endl;
		TFile* f = new TFile(paths[iFile], "read");
			
		for(Module& m : modules){

			TString nameOfModuleHistogram = "hTotVsTDiff_module_" + Convert::toNdigit(m.getID(), 3);
			array<TH1F*,2> pmtHistograms = {nullptr, nullptr};
			
			// get the ToT histograms of the two channels which this module is connected to
			bool histogramMissing = false;
			for(int pmt = 0; pmt < 2; pmt++){
				TString nameOfPMTHistogram = "hTot_ch_" + Convert::toNdigit(m.getChannels()[pmt], 3);
				pmtHistograms[pmt] = (TH1F*) f->Get(nameOfPMTHistogram);
				if(!pmtHistograms[pmt]){
					histogramMissing = true;
					break;
				}
			}
			
			// skip this module if any of the histograms is missing
			if(histogramMissing) continue;

			// get the ToT-vs.-tDiff correlation plot of this module
			TH2F* histogramFromFile = (TH2F*) f->Get(nameOfModuleHistogram);

			// skip this module if any of the histograms is missing
			if(!histogramFromFile) continue;

			// pass the histograms to the current instance of class Module
			m.fill(iVoltage, histogramFromFile, pmtHistograms);
		}
		f->Close();

	}
	return;
}
