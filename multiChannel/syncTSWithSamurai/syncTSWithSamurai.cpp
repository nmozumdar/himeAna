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

#include "syncTSWithSamurai.h"
#include "TDiffData.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdint>
using std::cout;
using std::endl;
using std::vector;

void syncTSWithSamurai(const char* trb3dir, const char* tDiffDir, const char* tDiffFilename, 
                     const char* mainDAQDir, const char* mainDAQFilename, const char* outputDir, const char* outputFilename){

	// ---------- Load tDiff input using TDiffData ----------
	TDiffData tDiffInput(TString(trb3dir) + "/data/tDiff/" + TString(tDiffDir), tDiffFilename);
	int nTDiff = tDiffInput.getNEvents();
	
	// ---------- Open main DAQ file ----------
	TString mainDAQPath = TString(mainDAQDir) + "/" + TString(mainDAQFilename);
	cout << "[syncTSWithSamurai] Opening main DAQ file: " << mainDAQPath << endl;
	TFile *fMainDAQ = TFile::Open(mainDAQPath);
	if (!fMainDAQ || fMainDAQ->IsZombie()) {
		cout << "[syncTSWithSamurai] Error opening main DAQ file!" << endl;
		return;
	}
	
	TTree *tMainDAQ = (TTree*)fMainDAQ->Get("t_TS");
	if (!tMainDAQ) {
		cout << "[syncTSWithSamurai] Error: t_TS tree not found!" << endl;
		return;
	}
	
	ULong64_t SMTS;
	tMainDAQ->SetBranchAddress("SMTS", &SMTS);
	int nMainDAQ = tMainDAQ->GetEntries();
	cout << "[syncTSWithSamurai] Main DAQ events: " << nMainDAQ << endl;
	
	TString outputPath = TString(trb3dir) + "/data/" + TString(outputDir) + "/" + TString(outputFilename);
	TFile *fOut = new TFile(outputPath, "recreate");
	TTree *tOut = new TTree("tree", "tree");
	
	vector<float> outTDiff, outTSum, outTofRaw, outTot0, outTot1, outT0, outT1;
	vector<int> outModuleID;
	int outNHits;
	uint64_t outSlowScaler, outFastScaler, outEventNumber;
	
	tOut->Branch("tDiff", &outTDiff);
	tOut->Branch("tSum", &outTSum);
	tOut->Branch("tofRaw", &outTofRaw);
	tOut->Branch("tot0", &outTot0);
	tOut->Branch("tot1", &outTot1);
	tOut->Branch("t0", &outT0);
	tOut->Branch("t1", &outT1);
	tOut->Branch("moduleID", &outModuleID);
	tOut->Branch("nHits", &outNHits, "nHits/I");
	tOut->Branch("slowScaler", &outSlowScaler, "slowScaler/g");
	tOut->Branch("fastScaler", &outFastScaler, "fastScaler/g");
	tOut->Branch("eventNumber", &outEventNumber, "eventNumber/g");
	
	const int64_t TOLERANCE = 200;
	const int64_t SCALER1_SCALE = 40;
	const int64_t SMTS_SCALE = 10;
	
	int tDiffIdx = 0;
	int matchesFound = 0;
	int emptyEventsInserted = 0;
	int existingEmptyPreserved = 0;
	
	for (int mainDAQIdx = 0; mainDAQIdx < nMainDAQ; mainDAQIdx++) {
		tMainDAQ->GetEntry(mainDAQIdx);
		int64_t mainDAQTime = (int64_t)SMTS * SMTS_SCALE;
		outEventNumber = (uint64_t)mainDAQIdx;
		
		bool matched = false;
		
		if (tDiffIdx < nTDiff) {
			tDiffInput.getEvent(tDiffIdx);
			
			if (tDiffInput.fastScaler == 0) {
				// Empty event in tDiff - always preserve it
				outTDiff = *tDiffInput.tDiff;
				outTSum = *tDiffInput.tSum;
				outTofRaw = *tDiffInput.tofRaw;
				outTot0 = *tDiffInput.tot0;
				outTot1 = *tDiffInput.tot1;
				outT0 = *tDiffInput.t0;
				outT1 = *tDiffInput.t1;
				outModuleID = *tDiffInput.moduleID;
				outNHits = tDiffInput.nHits;
				outSlowScaler = tDiffInput.slowScaler;
				outFastScaler = tDiffInput.fastScaler;
				tOut->Fill();
				existingEmptyPreserved++;
				tDiffIdx++;
				matched = true;
				matchesFound++;
			} else {
				int64_t tDiffTime = (int64_t)tDiffInput.fastScaler * SCALER1_SCALE;
				int64_t timeDiff = mainDAQTime - tDiffTime;
				
				if (abs(timeDiff) <= TOLERANCE) {
					outTDiff = *tDiffInput.tDiff;
					outTSum = *tDiffInput.tSum;
					outTofRaw = *tDiffInput.tofRaw;
					outTot0 = *tDiffInput.tot0;
					outTot1 = *tDiffInput.tot1;
					outT0 = *tDiffInput.t0;
					outT1 = *tDiffInput.t1;
					outModuleID = *tDiffInput.moduleID;
					outNHits = tDiffInput.nHits;
					outSlowScaler = tDiffInput.slowScaler;
					outFastScaler = tDiffInput.fastScaler;
					tOut->Fill();
					tDiffIdx++;
					matched = true;
					matchesFound++;
				}
			}
		}
		
		if (!matched) {
			cout << "[syncTSWithSamurai] Warning: No tDiff event found for main DAQ event " << mainDAQIdx 
			     << " (mainDAQTime=" << mainDAQTime << ")" << endl;
			outTDiff.clear();
			outTSum.clear();
			outTofRaw.clear();
			outTot0.clear();
			outTot1.clear();
			outT0.clear();
			outT1.clear();
			outModuleID.clear();
			outNHits = 0;
			outSlowScaler = 0;
			outFastScaler = 0;
			tOut->Fill();
			emptyEventsInserted++;
		}
	}
	
	fOut->cd();
	tOut->Write();
	fOut->Close();
	
	fMainDAQ->Close();
	
	cout << "[syncTSWithSamurai] ====================" << endl;
	cout << "[syncTSWithSamurai] Synchronization complete!" << endl;
	cout << "[syncTSWithSamurai] Main DAQ events processed: " << nMainDAQ << endl;
	cout << "[syncTSWithSamurai] Matches found: " << matchesFound << endl;
	cout << "[syncTSWithSamurai] Empty events inserted: " << emptyEventsInserted << endl;
	cout << "[syncTSWithSamurai] Existing empty events from DABC : " << existingEmptyPreserved << endl;
	cout << "[syncTSWithSamurai] Total tDiff events processed: " << tDiffIdx << " / " << nTDiff << endl;
	cout << "[syncTSWithSamurai] Output file: " << outputPath << endl;
	cout << "[syncTSWithSamurai] ====================" << endl;
}
