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

#include <iostream>
#include "Constants.h"
#include "TFile.h"

using std::vector;
using MF = hadaq::MessageFloat;

HistogramCollection::HistogramCollection(const vector<int>& activeChannels, int nEvents){

	this->fActiveChannels = activeChannels;
	fInvNEvents = 1. / nEvents;
	const int nCh = Constants::nChTot;

	previousFastScaler = 0;
	previousSlowScaler = 0;

	hChannels      = TH1F("hChannels", "TDC channels", nCh, 0, nCh);
	hTimeStamps    = TH1F("hTimeStamps", "Time stamps", 600, -300, 300);
	hTrigger       = TH1F("hTrigger", "Trigger signals", 16, 0, 16);
	hChVsEvtNr     = TH2F("hChVsEvtNr", "Number of messages from each channel in dependence on event number", 100, 0, 100, nCh, 0, nCh);
	hNMessages     = TH1F("hNMessages", "Number of messages per event", 50, 0, 50);
	hChCorr        = TH2F("hChCorr", "Number of hits in each pair of channels", Constants::nChTot, 0, Constants::nChTot, Constants::nChTot, 0, Constants::nChTot);
	hTotVsModuleID = TH2F("hTotVsModuleID", "Time over threshold vs. module ID", Constants::nModules, 0, Constants::nModules, 70, 0, 35);
	hTDiffVsModuleID = TH2F("hTDiffVsModuleID", "", Constants::nModules, 0, Constants::nModules, 320, -40, 40);
	hTofVsModuleID = TH2F("hTofVsModuleID", "", Constants::nModules, 0, Constants::nModules, 700, -150, 550);
	hScalerDiff     = TH1F("hScalerDiff", "Difference between fast scaler and 4x slow scaler", 200, -100, 100);
	hChannels.GetXaxis()->SetTitle("Channel number");
	hTimeStamps.GetXaxis()->SetTitle("Time (ns)");
	hTrigger.GetXaxis()->SetTitle("Tigger (TrbNet Type)");
	hChVsEvtNr.GetXaxis()->SetTitle("Event number (%)");
	hChVsEvtNr.GetYaxis()->SetTitle("Channel number");
	hNMessages.GetXaxis()->SetTitle("Number of messages");
	hChCorr.GetXaxis()->SetTitle("Channel number");
	hChCorr.GetYaxis()->SetTitle("Channel number");
	hTotVsModuleID.GetXaxis()->SetTitle("Module ID");
	hTotVsModuleID.GetYaxis()->SetTitle("ToT (ns)");
	gSlowScalerDiff.SetName("gSlowScalerDiff");
	gFastScalerDiff.SetName("gFastScalerDiff");
	gSlowScalerDiff.GetXaxis()->SetTitle("Event number");
	gSlowScalerDiff.GetYaxis()->SetTitle("TBT (s)");
	gFastScalerDiff.GetXaxis()->SetTitle("Event number");
	gFastScalerDiff.GetYaxis()->SetTitle("TBT (s)");
}



void HistogramCollection::fill(const vector<vector<MF*>>& messagesSortedByChannel, int trigger, uint64_t slowScaler, uint64_t fastScaler, int eventCounter){
	// fill histograms showing general information about the hadaq::MessageFloat objects
	// that were recorded in this run
	int nMessages = 0;
	for(int ch: fActiveChannels){
		const vector<MF*>& messages = messagesSortedByChannel[ch];
		nMessages += messages.size();
		hChannels.Fill(ch, messages.size());
		hChVsEvtNr.Fill(100. * eventCounter * fInvNEvents, ch, messages.size());
		for(const MF* mf: messages){
			hTimeStamps.Fill(mf->getStamp());
		}
	}
	hNMessages.Fill(nMessages);
	hTrigger.Fill(trigger);
	hScalerDiff.Fill(fastScaler - 4. * slowScaler);

	// iterate over all pairs of active channels
	for(int i = 0; i < fActiveChannels.size(); i++){
		for(int j = i; j < fActiveChannels.size(); j++){
			int ch0 = fActiveChannels[i];
			int ch1 = fActiveChannels[j];
			// fill the histogram, if there was at least one hit in both channels
			if(messagesSortedByChannel[ch0].size() > 0 && messagesSortedByChannel[ch1].size() > 0){
				hChCorr.Fill(ch0, ch1);
			}
		}
	}

	// calculate the difference between the scalers of the current and previous events
	if(eventCounter > 1){
		gSlowScalerDiff.SetPoint(gSlowScalerDiff.GetN(), eventCounter, (slowScaler - previousSlowScaler)*40e-9);
		gFastScalerDiff.SetPoint(gFastScalerDiff.GetN(), eventCounter, (fastScaler - previousFastScaler)*10e-9);
	}
	previousSlowScaler = slowScaler;
	previousFastScaler = fastScaler;
}



void HistogramCollection::fill(float tot, float tDiff, float tof, int moduleID){
	hTotVsModuleID.Fill(moduleID, tot);
	hTDiffVsModuleID.Fill(moduleID, tDiff);
	hTofVsModuleID.Fill(moduleID, tof);
}




void HistogramCollection::write(TFile* f){

	f->cd();
	hChannels.Write();
	hTimeStamps.Write();
	hTrigger.Write();
	hChVsEvtNr.Write();
	hNMessages.Write();
	hChCorr.Write();
	hScalerDiff.Write();
	gSlowScalerDiff.Write();
	gFastScalerDiff.Write();
	hTotVsModuleID.Write();
	hTDiffVsModuleID.Write();
	hTofVsModuleID.Write();
}