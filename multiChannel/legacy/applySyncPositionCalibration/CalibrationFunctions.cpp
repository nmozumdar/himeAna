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

#include "CalibrationFunctions.h"
#include "Constants.h"
#include "TH1F.h"
#include "TMath.h"
#include <iostream>
using std::cout;
using std::endl;
using std::vector;
using std::to_string;



CalibrationFunctions::CalibrationFunctions(TString path){
 	
	cout << "[CalibrationFunctions] Reading file " << path.Data() << endl;
	file = new TFile(path, "read");
	posCalFuncs = vector<TF1*>(Constants::nModules);
	tSync = vector<double>(Constants::nModules);
	TH1F* hVEff = (TH1F*) file->Get("hVEff");
	TH1F* hOffs = (TH1F*) file->Get("hOffs");
	TH1F* htSync = (TH1F*) file->Get("htSync");

	for(int m = 0; m < Constants::nModules; m++){
		TString name(TString("module_") + to_string(m).data() + TString("_posCalFunc"));
		posCalFuncs[m] = new TF1(name, "[0] * x + [1]", -500., 500.);
		posCalFuncs[m]->SetParameter(0, 0.5 * hVEff->GetBinContent(m+1));
		posCalFuncs[m]->SetParameter(1, hOffs->GetBinContent(m+1));
		tSync[m] = htSync->GetBinContent(m+1);
	}
}



float CalibrationFunctions::operator()(int moduleID, float tDiff){
	return getCalibratedValue(moduleID, tDiff);
}



float CalibrationFunctions::getCalibratedValue(int moduleID, float tDiff){
	if(posCalFuncs[moduleID]->GetParameter(0) == 0.) return -10000.;
	return posCalFuncs[moduleID]->Eval(tDiff);
}

float CalibrationFunctions::getTDiffCorr(int moduleID, float tDiff){
	if(posCalFuncs[moduleID]->GetParameter(0) == 0.) return -10000.;
	return tDiff + posCalFuncs[moduleID]->GetParameter(1)/posCalFuncs[moduleID]->GetParameter(0);
}
float CalibrationFunctions::getTMeanCorr(int moduleID, float tSum){
	if(TMath::IsNaN(tSync[moduleID])) return -10000;
	return tSum/2. - tSync[moduleID];
}
