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
#include "TH1.h"
#include "TFile.h"
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



void tDiff(const char *trb3dir, const char *dir, const char *filename, const char* channelMapping, const char* calfile, int trigger, bool multihit, bool write, bool plot){

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

	// ---------------- Calibration file for multihit ----------------

	TFile *calFile = new TFile(TString(trb3dir) + "/calibration/calibrationParameter/"+ calfile + "/calibration.root");
        TH1F* hVEff = (TH1F*)calFile->Get("hVEff");
        TH1F* hOffs = (TH1F*)calFile->Get("hOffs");
	
	// ---------------- Loop over events ----------------

	const int nEvents = 1e6;//input.getNEvents();

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
		// TODO: Check 
		vector<array<float,2>> referencePulselu;
		vector<array<float,2>> referencePulserd;
		float referencePulse = 0./0.;
		bool refPulseFound = false;
		bool startPulseFound = PulseAna::findPulses(messagesSortedByChannel[modules[288].getChLeftUp()], referencePulselu, 288) && PulseAna::findPulses(messagesSortedByChannel[modules[288].getChRightDown()], referencePulserd, 288);

		while(startPulseFound)
		{
			bool pileup = false;
			double window[2] = {Constants::sbt_lr[0], Constants::sbt_lr[1]}; 

			std::vector<bool> setlu(referencePulselu.size(),false);
			std::vector<bool> setrd(referencePulserd.size(),false);
			if(!multihit)
			{
				refPulseFound = true;
				referencePulse = (referencePulselu[0][0] + referencePulserd[0][0])/2.;
				break;
			}

			for(int i = 0; i < referencePulselu.size(); i++)
			{
				const array<float,2>& timeStamps_left_up = referencePulselu[i];

				for(int j = 0; j < referencePulserd.size(); j++)
				{
					const array<float,2>& timeStamps_right_down = referencePulserd[j];
					float tDiff = timeStamps_left_up[0] - timeStamps_right_down[0];

					if(multihit && !(tDiff > window[0] && tDiff < window[1]))
						continue;

					if(setlu[i] || setrd[j])
					{
						//std::cout<<"Pileup LR event"<<std::endl;
						pileup = true;
						refPulseFound = false;
						referencePulse = 0./0.;
						break;
					}
					//	std::cout<<"here: "<<tDiff<<std::endl;

					setlu[i] = true;
					setrd[j] = true;
					if(timeStamps_left_up[0] > Constants::sbt_win[0] && timeStamps_left_up[0] > Constants::sbt_win[0])
					{
						if(refPulseFound)
						{
							//std::cout<<"Pileup trigger event"<<std::endl;
							pileup = true;
							refPulseFound = false;
							referencePulse = 0./0.;
							break;
						}
						refPulseFound = true;
						referencePulse = (timeStamps_right_down[0] + timeStamps_left_up[0])/2.;
					}
				}
				if(pileup)
					break;
			}
			break;
		}
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
				PulseAna::findPulses(messages_left_up, pulses_left_up, m.getID()) && \
				PulseAna::findPulses(messages_right_down, pulses_right_down, m.getID());
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
			if(moduleHasHits)
			{
				bool pileup = false;
				double window[2];
				if(m.getID() < 288)
				{
					window[0] = Constants::himep_lr[0],
					window[1] = Constants::himep_lr[1];
				}
				else if(m.getID() == 288)
				{
					window[0] = Constants::sbt_lr[0],
					window[1] = Constants::sbt_lr[1];
				}
				else if(m.getID() > 288)
				{

					window[0] = Constants::hime_lr[0],
					window[1] = Constants::hime_lr[1];
				}

				std::vector<bool> setlu(pulses_left_up.size(),false);
				std::vector<bool> setrd(pulses_right_down.size(),false);

				for(int i = 0; i < pulses_left_up.size(); i++)
				{
					const array<float,2>& timeStamps_left_up = pulses_left_up[i];

					for(int j = 0; j < pulses_right_down.size(); j++)
					{
						const array<float,2>& timeStamps_right_down = pulses_right_down[j];
						double offset = 0;
						float tDiff = timeStamps_left_up[0] - timeStamps_right_down[0];
						if(hVEff && hOffs && m.getID() != 288)
							offset = 2*hOffs->GetBinContent(m.getID()+1)/hVEff->GetBinContent(m.getID()+1);
						if(multihit && !(tDiff+offset > window[0] && tDiff+offset < window[1]))
							continue;

						if(setlu[i] || setrd[j])
						{
							//std::cerr<<"Pileup LR event"<<std::endl;
							pileup = true;
							break;
						}

						setlu[i] = true;
						setrd[j] = true;

						// ToT of the PMT on the right/bottom side
						float tot_right_down = timeStamps_right_down[1] - timeStamps_right_down[0];
						output.tot0.push_back(tot_right_down);
						// ToT of the PMT on the left/top side
						float tot_left_up = timeStamps_left_up[1] - timeStamps_left_up[0];
						output.tot1.push_back(tot_left_up);
						// time difference of the two rising edges
						output.tDiff.push_back(tDiff);
						// sum of the time stamps of the two rising edges
						float tSum = timeStamps_right_down[0] + timeStamps_left_up[0];
						output.tSum.push_back(tSum);
						// uncalibrated ToF
						float tof;
						if(refPulseFound)
							tof = tSum/2. - referencePulse;
						else
							tof = 0./0.;
						output.tofRaw.push_back(tof);
						// ID of the module that was hit
						output.moduleID.push_back(m.getID());
						// count the number of hits in this event
						output.nHits++;
						// Time of the PMT on the right/bottom side
						output.t0.push_back(timeStamps_right_down[0]);
						// Time of the PMT on the left/top side
						output.t1.push_back(timeStamps_left_up[0]);
						// fill histograms that are required for the further analysis,
						// such as position calibration and gain matching
						m.fillHistograms(tDiff, tot_left_up, tot_right_down);
						hc.fill(TMath::Sqrt(tot_left_up*tot_right_down), tDiff, tof, m.getID());
					}
					if(pileup)
						break;
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
