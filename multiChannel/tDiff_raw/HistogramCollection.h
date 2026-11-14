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

#ifndef HistogramCollection_h
#define HistogramCollection_h

#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TFile.h"
#include "TdcSubEvent.h"
#include "Detector.h"

class HistogramCollection{

	public:
	HistogramCollection();
	HistogramCollection(const std::vector<int>& activeChannels, int nEvents);
	void fill(const std::vector<std::vector<hadaq::MessageFloat*>>& messagesSortedByChannel, int trigger, uint64_t slowScaler, uint64_t fastScaler, int eventCounter);
	void fill(float tot, float tDiff, float tof, int moduleID);
	void write(TFile* f);

	TH1F hChannels;
	TH1F hTimeStamps;
	TH1F hTrigger;
	TH2F hChVsEvtNr;
	TH1F hNMessages;
	TH2F hChCorr;
	TH1F hScalerDiff;
	TH2F hTotVsModuleID;
	TH2F hTDiffVsModuleID;
	TH2F hTofVsModuleID;
	TGraph gSlowScalerDiff;
	TGraph gFastScalerDiff;

	private:
	std::vector<int> fActiveChannels;
	float fInvNEvents;
	uint64_t previousFastScaler;
	uint64_t previousSlowScaler;
};

#endif
