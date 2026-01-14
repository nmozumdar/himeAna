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

/* 
 * Analyze hadaq::MessageFloat objects for all modules to prepare the data for calibration.
 * The results are written to a TTree.
 */

#include "tDiff.h"

#include <vector>
#include <iostream>
#include <fstream>

#include "ProgressIndicator.h"
#include "TRB3RawData.h"
#include "HistogramCollection.h"
#include "TdcSubEvent.h"
#include "TDiffData.h"
#include "Detector.h"
#include "Module.h"
#include "PulseAna.h"
#include "TMath.h"
#include "TGraph.h"

using std::cout;
using std::endl;
using std::array;
using std::vector;
using MF = hadaq::MessageFloat;

//     _______________
//     | .       ~   |
//     |  HIME ~   . |
//     | .~  rocks  .|
//     |_____________|
//            |
//      |\/\  |
//      (^.^) |
//     ,/   \-o
//  ----------------------------  



void tDiff(const char *trb3dir, const char *dir, const char *filename, const char* channelMapping, int trigger, bool multihit, bool write, bool plot){

	// ---------------- Input ----------------

	TString pathInput(TString(trb3dir) + "/data/unpacked/" + TString(dir) + "/" + TString(filename));
	TRB3RawData input(pathInput);

	// ---------------- Output ----------------

	TString pathOutput(TString(trb3dir) + "/data/tDiff/" + TString(dir) + "/" + TString(filename));
	TDiffData output(pathOutput);

	// ---------------- Channel mapping ----------------

	// Create a std::vector of Module objects, where each Module has an ID and two channels.
	// In each channel, there will be sequences of hadaq::MessageFloat objects,
	// representing the signals of a PMT.
	vector<Module> modules = Detector::build("../../data/channelMapping/" + TString(channelMapping));
	vector<int> activeChannels = Detector::getActiveChannels(modules);
	std::sort(activeChannels.begin(), activeChannels.end());

	// ---------------- Loop over events ----------------

	const int nEvents = input.getNEvents();

	HistogramCollection hc(activeChannels, nEvents);
	ProgressIndicator pi(nEvents, "[tDiff] Processed events:");

	 //for(int eventCounter = 0; eventCounter < 100; eventCounter++){
	for(int eventCounter = 0; eventCounter < nEvents; eventCounter++){
		output.eventNumber = eventCounter; 
		pi.showProgress(eventCounter);
		output.reset();
		vector<vector<MF*>> messagesSortedByChannel = input.getMessagesSortedByChannel(eventCounter);

		// check trigger
		if(trigger > -1){
			if( trigger != input.getTrigger() ) continue;
		}

		hc.fill(messagesSortedByChannel, input.getTrigger(), input.getSlowScaler(), input.getFastScaler(), eventCounter);
		
		// copy scalers
		output.slowScaler = input.getSlowScaler();
		output.fastScaler = input.getFastScaler();

		// determine the reference time to calculate the uncalibrated ToF
		std::array<float,2> referencePulse;
		bool refTimeFound = PulseAna::findPulse(messagesSortedByChannel[0], referencePulse);

		// loop over all modules of the detector
		for(Module& m: modules){
			// get all MessageFloat objects of each PMT
			vector<MF*>& messages_left_up = messagesSortedByChannel[m.getChLeftUp()];
			vector<MF*>& messages_right_down = messagesSortedByChannel[m.getChRightDown()];

			// If a rising signal is found that is followed by a falling signal,
			// the respective time stamps are written to the arrays inside
			// the following vectors. 
			// There can be multiple good signals in each PMT.
			vector<array<float,2>> pulses_left_up;
			vector<array<float,2>> pulses_right_down;

			// If both PMTs have a rising signal that is followed by a falling one,
			// we have a complete hit in the current module,
			// which has information about position and energy deposition
			// of the particle that has hit the detector.
			bool moduleHasHits = false;

			if(multihit){
				moduleHasHits = \
				PulseAna::findPulses(messages_left_up, pulses_left_up) && \
				PulseAna::findPulses(messages_right_down, pulses_right_down);
			}
			// use only the first complete pulse in each PMT
			// initialize an array where the rising and falling edges
			// of the first pulse in each PMT can be stored
			else{
				array<float,2> empty = {0., 0.};
				pulses_left_up.push_back(empty);
				pulses_right_down.push_back(empty);
				moduleHasHits = \
				PulseAna::findPulse(messages_left_up, pulses_left_up[0]) && \
				PulseAna::findPulse(messages_right_down, pulses_right_down[0]);
			}


			// Take only such cases into account, where the number of
			// pulses is equal in both PMTs.
			if(
				moduleHasHits && \
				pulses_left_up.size() == pulses_right_down.size()
			){
				for(int i = 0; i < pulses_left_up.size(); i++){
					const array<float,2>& timeStamps_left_up = pulses_left_up[i];
					const array<float,2>& timeStamps_right_down = pulses_right_down[i];
					// time difference of the two rising edges
					float tDiff = timeStamps_left_up[0] - timeStamps_right_down[0];
					output.tDiff.push_back(tDiff);
					// sum of the time stamps of the two rising edges
					float tSum = timeStamps_right_down[0] + timeStamps_left_up[0];
					output.tSum.push_back(tSum);
					// uncalibrated ToF
					float tof = tSum/2. - referencePulse[0];
					output.tofRaw.push_back(tof);
					// ToT of the PMT on the left/top side
					float tot_left_up = timeStamps_left_up[1] - timeStamps_left_up[0];
					output.tot0.push_back(tot_left_up);
					// ToT of the PMT on the right/bottom side
					float tot_right_down = timeStamps_right_down[1] - timeStamps_right_down[0];
					output.tot1.push_back(tot_right_down);
					// ID of the module that was hit
					output.moduleID.push_back(m.getID());
					// count the number of hits in this event
					output.nHits++;
					// fill histograms that are required for the further analysis,
					// such as position calibration and gain matching
					m.fillHistograms(tDiff, tot_left_up, tot_right_down);
					hc.fill(TMath::Sqrt(tot_left_up*tot_right_down), tDiff, tof, m.getID());
				}
			}
		}
		output.fill();
	}
	cout << endl;
	
	output.write();
	hc.write(output.getFile());
	for(const Module& m: modules) m.write(output.getFile());

}
