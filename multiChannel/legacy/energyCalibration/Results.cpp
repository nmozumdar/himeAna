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

#include "Results.h"
#include "Constants.h"
#include "Calculator.h"
#include "Convert.h"
using std::vector;



Results::Results(){
	// Initialize histogram showing the energy resolution for all modules
	hEnergyResolution = TH1F("hEnergyResolution", "", Constants::nModules, 0, Constants::nModules);
	hEnergyResolution.GetYaxis()->SetTitle("Resolution (%)");


	// Initialize histograms showing the fit parameters of the energy-calibration functions
	hVecFitPar = vector<TH1F>(3);
	titles = {"Constant", "Slope", "Curvature"};
	// loop over fit paramters
	for(int ip = 0; ip < hVecFitPar.size(); ip++){
		hVecFitPar[ip] = TH1F("hFitPar" + Convert::toStr(ip), titles[ip], Constants::nModules, 0, Constants::nModules);
	}
}



void Results::saveFitResult(Module& m, double energy_at_which_to_calculate_dEPerE){

	hEnergyResolution.SetTitle("Resolution at " + Convert::toStr(energy_at_which_to_calculate_dEPerE) + " MeV");
	Calculator::relativeResolution(m, energy_at_which_to_calculate_dEPerE);
	hEnergyResolution.SetBinContent(m.iMod + 1, 100. * m.relativeResolution);

	int nParameters = m.isHorizontal ? 2 : 3;
	for(int iPar = 0; iPar < nParameters; iPar++){
		hVecFitPar[iPar].SetBinContent(m.iMod + 1, m.calibrationFunction->GetParameter(iPar));
	}
}