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
#include "Convert.h"
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

	ECalFuncs = vector<TF1*>(Constants::nModules);

	if (hVEff && hOffs && htSync)
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

			ECalFuncs[m] = (TF1*)file->Get("calibrationFunction" + Convert::toStr(m));
			if (ECalFuncs[m] == nullptr)
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

float CalibrationFunctions::getTofCorr(int moduleID, float tofRaw)
{
	if(TMath::IsNaN(tSync[moduleID])) return -10000;
	return tofRaw - tSync[moduleID];

}
float CalibrationFunctions::getPmtTCorr(int moduleID, float pmtT, int side)
{
	if(TMath::IsNaN(tSync[moduleID]) || posCalFuncs[moduleID]->GetParameter(0) == 0.) return -10000;
	double offset = tSync[moduleID] + (0.5 - side)*posCalFuncs[moduleID]->GetParameter(1)/posCalFuncs[moduleID]->GetParameter(0);
	return pmtT - offset;

}
float CalibrationFunctions::getECalibratedValue(int moduleID, float avgToT)
{
	if(!ECalFuncs[moduleID]) return -10000.;
        return ECalFuncs[moduleID]->Eval(avgToT);
}
