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
#include "Constants.h"
#include "Convert.h"
#include <iostream>
using std::vector;
using std::cout;
using std::endl;



HistogramCollection::HistogramCollection(){
	hVEff = TH1F("hVEff", "Effective velocity of light", Constants::nModules, 0, Constants::nModules);
	hOffs = TH1F("hOffs", "Position offset", Constants::nModules, 0, Constants::nModules);

	htSync = TH1F("hTSync", "tSync Values", Constants::nModules, 0, Constants::nModules);

	hVEff.GetXaxis()->SetTitle("Module ID");
	hOffs.GetXaxis()->SetTitle("Module ID");
	htSync.GetXaxis()->SetTitle("Module ID");

	hVEff.GetYaxis()->SetTitle("Effective velocity of light (mm/ns)");
	hOffs.GetYaxis()->SetTitle("Position offset (mm)");
	htSync.GetYaxis()->SetTitle("tSync (ns)");


	// Initialize histograms showing the fit parameters of the energy-calibration functions
        hEnergyFitPar = vector<TH1F>(3);
        titles = {"Constant", "Slope", "Curvature"};
        // loop over fit paramters
        for(int ip = 0; ip < hEnergyFitPar.size(); ip++){
                hEnergyFitPar[ip] = TH1F("hEnergyFitPar" + Convert::toStr(ip), titles[ip], Constants::nModules, 0, Constants::nModules);
        }

}



void HistogramCollection::fill(vector<Module>& modules){
	for(int id = 0; id < Constants::nModules; id++){
		if(!std::isnan(modules[id].tSync.Value))
		{
			htSync.SetBinContent(id+1, modules[id].tSync.Value);
			htSync.SetBinError(id+1, modules[id].tSync.Error);
		}

		nParameters = modules[id].nEPar;
		
		if(modules[id].EcalibrationSuccessful) 
		{
			for(int iPar = 0; iPar < nParameters; iPar++){
				hEnergyFitPar[iPar].SetBinContent(id + 1, modules[id].calibrationFunction->GetParameter(iPar));
			}
		}

		if(!modules[id].calibrationSuccessful) continue;
		hVEff.SetBinContent(id+1, modules[id].getEffectiveVelocity());
		hOffs.SetBinContent(id+1, modules[id].getPositionOffset());
		hVEff.SetBinError(id+1, modules[id].getEffectiveVelocityUnc());
		hOffs.SetBinError(id+1, modules[id].getPositionOffsetUnc());
	}
}



void HistogramCollection::write(TFile* file){
	file->cd();
	hVEff.Write();
	hOffs.Write();
	htSync.Write();

	for(int iPar = 0; iPar < nParameters; iPar++){
		hEnergyFitPar[iPar].Write();
	}
}
