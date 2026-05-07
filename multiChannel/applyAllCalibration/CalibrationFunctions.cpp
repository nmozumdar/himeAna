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

	// ===== Load Sync calibration parameters from histograms =====

	TH1F *htSync = (TH1F *)file->Get("htSync");

	// ===== Load energy calibration parameters from histograms =====

	TH1F *hPar0 = (TH1F *)file->Get("hEnergyFitPar0");
	TH1F *hPar1 = (TH1F *)file->Get("hEnergyFitPar1");

	if (hVEff && hOffs && htSync && hPar0 && hPar1)
	{
		for (int m = 0; m < Constants::nModules; m++)
		{
			// Position calibration
			double vEff = 0.5 * hVEff->GetBinContent(m + 1);
			double offs = hOffs->GetBinContent(m + 1);
			TString posFuncName = TString("posCalFunc_") + to_string(m).data();
			TF1 *posFunc = new TF1(posFuncName, "[0] * x + [1]", -500., 500.);
			posFunc->SetParameter(0, vEff);
			posFunc->SetParameter(1, offs);
			posCalFuncs[m] = posFunc;

			// Sync calibration
			tSync[m] = htSync->GetBinContent(m + 1);

			// Energy calibration
			double par0 = hPar0->GetBinContent(m + 1);
			double par1 = hPar1->GetBinContent(m + 1);
			if (par0 != 0. || par1 != 0.)
			{
				TString energyFuncName = TString("calibrationFunction") + to_string(m).data();
				TF1 *energyFunc = new TF1(energyFuncName, "[0] + [1] * x", 0, 1000);
				energyFunc->SetParameter(0, par0);
				energyFunc->SetParameter(1, par1);
				ECalFuncs[m] = energyFunc;
			}
		}
	}
	else
	{
		cout << "[CalibrationFunctions] Warning: Missing histograms for calibration parameters" << endl;
	}

	cout << "[CalibrationFunctions] Loaded " << posCalFuncs.size() << " position calibration functions" << endl;
	cout << "[CalibrationFunctions] Loaded " << tSync.size() << " sync calibration parameters" << endl;
	cout << "[CalibrationFunctions] Loaded " << ECalFuncs.size() << " energy calibration functions from histograms" << endl;
}

float CalibrationFunctions::getCalibratedValue_p(int moduleID, float tDiff)
{
	auto it = posCalFuncs.find(moduleID);
	if (it == posCalFuncs.end())
		return -10000.;
	if (!it->second)
		return -10000.;
	if (it->second->GetParameter(0) == 0.)
		return -10000.;
	return it->second->Eval(tDiff);
}

float CalibrationFunctions::getTDiffCorr(int moduleID, float tDiff)
{
	auto it = posCalFuncs.find(moduleID);
	if (it == posCalFuncs.end())
		return -10000.;
	if (!it->second)
		return -10000.;
	if (it->second->GetParameter(0) == 0.)
		return -10000.;
	return tDiff + it->second->GetParameter(1) / it->second->GetParameter(0);
}

float CalibrationFunctions::getTMeanCorr(int moduleID, float tSum)
{
	auto it = tSync.find(moduleID);
	if (it == tSync.end())
		return -10000;
	if (TMath::IsNaN(it->second))
		return -10000;
	return tSum / 2. - it->second;
}

float CalibrationFunctions::getCalibratedValue_e(int moduleID, float avgToT)
{
	auto it = ECalFuncs.find(moduleID);
	if (it == ECalFuncs.end())
		return -10000.;
	if (!it->second)
		return -10000.;
	return it->second->Eval(avgToT);
}
