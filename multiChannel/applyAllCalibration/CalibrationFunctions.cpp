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
#include "TGraphErrors.h"
#include "TKey.h"
#include <iostream>
using std::cout;
using std::endl;
using std::to_string;
using std::vector;

CalibrationFunctions::CalibrationFunctions(TString path)
{

	cout << "[CalibrationFunctions] Reading file " << path.Data() << endl;
	file = new TFile(path, "read");

	// ===== Load position calibration parameters from histograms =====
	TH1F *hVEff = (TH1F *)file->Get("hVEff");
	TH1F *hOffs = (TH1F *)file->Get("hOffs");
	posCalFuncs = vector<TF1*>(Constants::nModules);

	// ===== Load Sync calibration parameters from histograms =====

	TH1F *htSync = (TH1F *)file->Get("hTSync");
	tSync = vector<double>(Constants::nModules);

	// ===== Load energy calibration parameters from histograms =====

	TH1F *hPar0 = (TH1F *)file->Get("hEnergyFitPar0");
	TH1F *hPar1 = (TH1F *)file->Get("hEnergyFitPar1");
	ECalFuncs = vector<TF1*>(Constants::nModules);

	if (hVEff && hOffs && htSync && hPar0 && hPar1)
	{
		for (int m = 0; m < Constants::nModules; m++)
		{
			// Position/tDiff calibration
			double vEff = 0.5 * hVEff->GetBinContent(m + 1);
			double offs = hOffs->GetBinContent(m + 1);
			TString posFuncName(TString("module_") + to_string(m).data() + TString("_posCalFunc"));
			posCalFuncs[m] = new TF1(posFuncName, "[0] * x + [1]", -500., 500.);
			posCalFuncs[m]->SetParameter(0, vEff);
			posCalFuncs[m]->SetParameter(1, offs);

			// Sync calibration
			tSync[m] = htSync->GetBinContent(m + 1);

			// Energy calibration
			double par0 = hPar0->GetBinContent(m + 1);
			double par1 = hPar1->GetBinContent(m + 1);
			if (par0 != 0. || par1 != 0.)
			{
				TString energyFuncName(TString("module_") + to_string(m).data() + TString("_ECalFunc"));
				ECalFuncs[m] = new TF1(energyFuncName, "[0] + [1] * x", 0, 1000);
				ECalFuncs[m]->SetParameter(0, par0);
				ECalFuncs[m]->SetParameter(1, par1);
			}
			else
				cout << "[CalibrationFunctions] Warning: Missing Energy calibrationparameters for Module: " << m + 1 << endl;
		}
	}
	else
		cout << "[CalibrationFunctions] Warning: Missing histograms for calibration parameters" << endl;

}

float CalibrationFunctions::getPosCalibratedValue(int moduleID, float tDiff)
{
	if(posCalFuncs[moduleID]->GetParameter(0) == 0.) return -10000.;
	return posCalFuncs[moduleID]->Eval(tDiff);
}

float CalibrationFunctions::getTDiffCorr(int moduleID, float tDiff)
{
	if(posCalFuncs[moduleID]->GetParameter(0) == 0.) return -10000.;
	return tDiff + posCalFuncs[moduleID]->GetParameter(1)/posCalFuncs[moduleID]->GetParameter(0);
}

float CalibrationFunctions::getTMeanCorr(int moduleID, float tSum)
{
	if(TMath::IsNaN(tSync[moduleID])) return -10000;
	return tSum/2. - tSync[moduleID];

}

float CalibrationFunctions::getECalibratedValue(int moduleID, float avgToT)
{
	if(!ECalFuncs[moduleID]) return -10000.;
        return ECalFuncs[moduleID]->Eval(avgToT);
}
