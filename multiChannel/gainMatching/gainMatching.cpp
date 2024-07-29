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

#include "gainMatching.h"
#include "ProgressIndicator.h"
#include "TDiffData.h"
#include "GainAna.h"
#include "Output.h"
#include "SpectrumAna.h"
#include "HistogramCollection.h"
#include "Drawer.h"
#include "CSVReader.h"
#include <string>
#include <iostream>
using std::vector;
using std::string;
using std::cout;
using std::endl;



void gainMatching(
	const char* trb3dir, 
	std::vector<float> voltages, 
	std::vector<const char*> subdirs, 
	float desiredToT, 
	bool linearGainFitModel, 
	const char* outputSubdir, 
	const char* channelMappingFilename
){

	// Check if the number of given voltages matches the number of subdirectories
	if(voltages.size() != subdirs.size()){
		cout << "[gainMatching] The number of listed voltages does not match the number of subdirs!\n";
		cout << "voltages.size() = " << voltages.size() << "\n";
		cout << "subdirs.size() = " << subdirs.size() << endl;
		return;
	}

	// The instances of class Module contain (among other data):
	// - a ToT spectrum for each voltage
	// - a 2D correlation plot showing ToT vs. tDiff for each voltage
	// - a TGraph object correlating the position of the cosmic-muon peak with the voltage applied to the module
	vector<Module> modules;
	// Find which channels each module is connected to.
	// Create instances of class Module.
	TString channelMappingDir("../../data/channelMapping/");
	vector<vector<string>> channelMapping = CSVReader::read(channelMappingDir + channelMappingFilename, 7);
	
	for(int i = 0; i < channelMapping.size(); i++){
		
		int layer = std::stoi(channelMapping[i][3]);
		int moduleNumberInThisLayer = std::stoi(channelMapping[i][4]);
		int moduleID = layer * 24 + moduleNumberInThisLayer;

		int fpga = std::stoi(channelMapping[i][6]);
		int dacChain = std::stoi(channelMapping[i][5]);
		int channelOffset = fpga * 48 + dacChain * 16;
		int channelRight = channelOffset + std::stoi(channelMapping[i][1]);
		int channelLeft  = channelOffset + std::stoi(channelMapping[i][2]);
	
		modules.push_back(Module(moduleID, channelLeft, channelRight, voltages, linearGainFitModel));

		cout << "Adding module " << moduleID << ": " << channelLeft << ",  " << channelRight << endl;
	}
	
	// A collection of histograms showing general information about the gain-matching procedure
	HistogramCollection hc(voltages, desiredToT);

	// iterate over all voltages that have been applied to the modules
	// in the measurements where the input data for the gain-matching procedure are from
	for(int iVoltage = 0; iVoltage < voltages.size(); iVoltage++){

		cout << "\n[gainMatching] *** " << voltages[iVoltage] << " V ***\n" << endl;

		// ---------------- Input ----------------
		// import data from the subdirectories of data/tDiff
		TDiffData input(TString(trb3dir), TString(subdirs[iVoltage]));

		input.fillModules(modules, iVoltage);
		
		for(Module& m: modules){
			// check if the current module has counts
			if(m.gotHits()){
				// normalize all ToT spectra so that their integrals are 1
				m.normalizeToT();
				// extract the cosmic-muon peak position in the ToT spectrum for each voltage
				std::array<std::array<double,2>,3> peakPositionsAndUncertainties = SpectrumAna::fitLastPeak(m, iVoltage);
				// fill a TGraph that correlates the peak position with the voltage
				m.addPoint(iVoltage, peakPositionsAndUncertainties);
			}
		}
	}

	cout << endl;

	// Apply a fit to the TGraph (the one that correlates the ToT-peak position with the voltage)
	// and evaluate the fit funtion with the desired ToT value, where the cosmic-muon line should be located. 
	// This results in values for the voltage that need to be set for each module.
	for(Module& m: modules){
		if(m.gotHits()){
			GainAna::removeDeviatingPoints(m);
			GainAna::fit(m, desiredToT);
		}
	}

	// fill histograms that show general information for all modules
	hc.fill(modules);

	// Create a CSV file including all calculated target voltages,
	// which can be read by PowerSupplyControl
	// Write all histograms to a ROOT file.
	// Note that the Drawer writes TCanvas objects to this file.
	TFile* rootFileOut = Output::create(outputSubdir, modules);

	hc.write(rootFileOut);

	// Show histograms and write TCanvas objects to a file
	Drawer dr(voltages);
	dr.drawOverview(hc);
	for(Module& m: modules){
		if(m.gotHits()){
			cout << "module " << m.getID() << endl;
			dr.drawModule(m, true);
		}
	}
}
