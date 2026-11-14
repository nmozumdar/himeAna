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
#include "TF1.h"
#include "Constants.h"
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

	gInterpreter->GenerateDictionary("vector<pair<float,int>>", "vector;utility");

	// ---------------- Input ----------------

	TString pathInput(TString(trb3dir) + "/unpacked/" + TString(dir) + "/" + TString(filename));
	//TString pathInput(TString(trb3dir) + "/unpacked/" + TString(filename));
	TRB3RawData input(pathInput);

	// ---------------- Output ----------------

	TString pathOutput(TString(trb3dir) + "/tDiff/" + TString(dir) + "/" + TString(filename));
	//TString pathOutput(TString(trb3dir) + "/tDiff/" + TString(filename));
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

		// copy scalers
		output.slowScaler = input.getSlowScaler();
		output.fastScaler = input.getFastScaler();

		// determine the reference time to calculate the uncalibrated ToF
		std::array<float,2> referencePulse;
		bool refTimeFound = PulseAna::findPulse(messagesSortedByChannel[576], referencePulse);

		// loop over all modules of the detector
		for(Module& m: modules){
			// get all MessageFloat objects of each PMT
			vector<MF*>& messages_left_up = messagesSortedByChannel[m.getChLeftUp()];
			vector<MF*>& messages_right_down = messagesSortedByChannel[m.getChRightDown()];

			for(int iMsg = 0; iMsg < messages_right_down.size(); iMsg++){
				MF& mf = *(messages_right_down[iMsg]);

				if(mf.isRising())
				{
					output.moduleID.push_back(m.getID());
					// Time of the PMT on the right/bottom side
					//output.t0.push_back(mf.getStamp());
					output.t0.emplace_back(mf.getStamp(),m.getID());
				}
				else if(mf.isFalling())
				{
					// Time of the PMT on the right/bottom side
					// output.t0_t.push_back(mf.getStamp());
					output.t0_t.emplace_back(mf.getStamp(),m.getID());
				}
			}

			for(int iMsg = 0; iMsg < messages_left_up.size(); iMsg++){
				MF& mf = *(messages_left_up[iMsg]);

				if(mf.isRising())
				{
					// Time of the PMT on the left/top side
					//output.t1.push_back(mf.getStamp());
					output.t1.emplace_back(mf.getStamp(),m.getID());
				}
				else if(mf.isFalling())
				{
					// Time of the PMT on the left/top side
					// output.t1_t.push_back(mf.getStamp());
					output.t1_t.emplace_back(mf.getStamp(),m.getID());
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
