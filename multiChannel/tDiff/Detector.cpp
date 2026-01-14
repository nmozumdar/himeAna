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

#include "Detector.h"
#include "Constants.h"
#include "CSVReader.h"
#include <iostream>
using std::cout;
using std::endl;
using std::string;
using std::vector;



bool Detector::isInside(int i, const vector<int>& v){
	for(int element: v){
		if(i == element){
			return true;
		}
	}
	return false;
}



vector<Module> Detector::build(const char* path){

	vector<Module> modules;
	vector<vector<string>> csvData = CSVReader::read(path, 7);

	vector<int> seenModuleIDs;
	vector<int> seenChannels;
	
	// combine information and add modules to the detector
	for(const vector<string> &entries : csvData){

		int wall 		= std::stoi(entries[0]);
		int ch_r_d_raw 	= std::stoi(entries[1]);
		int ch_l_u_raw 	= std::stoi(entries[2]);
		int layer 		= std::stoi(entries[3]);
		int sub_module 	= std::stoi(entries[4]);
		//int chain 		= std::stoi(entries[5]);
		int tdc  		= std::stoi(entries[5]);
		int trb			= std::stoi(entries[6]);

		int tdcoff[3][4]={0,2,4,8,1,3,5,6,7,9,10,11};
		// determine the HIME-channel number (unique for each PMT)
		//int channelOffset = chain * 16 + tdc * 48 + wall * 1000;
		int channelOffset = tdcoff[trb-1][tdc] * 48 + wall * 1000;
		int ch_left_up = ch_l_u_raw + channelOffset;
		int ch_right_down = ch_r_d_raw + channelOffset;
		// determine the HIME-module ID (unique for each scintillator)
		int moduleID = layer * 24 + sub_module;

		if(isInside(moduleID, seenModuleIDs)){
			cout << "module ID " << moduleID << " appears more than once in the channel-mapping CSV file!" << endl;
			continue;
		}
		else{
			seenModuleIDs.push_back(moduleID);
		}
		if(isInside(ch_left_up, seenChannels)){
			cout << "ch_left_up " << ch_left_up << " appears more than once in the channel-mapping CSV file!" << endl;
			continue;
		}
		else{
			seenChannels.push_back(ch_left_up);
		}
		if(isInside(ch_right_down, seenChannels)){
			cout << "ch_right_down " << ch_right_down << " appears more than once in the channel-mapping CSV file!" << endl;
			continue;
		}
		else{
			seenChannels.push_back(ch_right_down);
		}
		modules.push_back(Module(moduleID, ch_left_up, ch_right_down));
	}

	return modules;
}



std::vector<int> Detector::getActiveChannels(const std::vector<Module>& modules){
	std::vector<int> activeChannels(2 * modules.size());

	// iterate over all modules
	for(int i = 0; i < modules.size(); i++){
		const Module& m = modules[i];
		// save the channel numbers of the right/bottom and the left/top PMT
		activeChannels[2*i] = m.getChRightDown();
		activeChannels[2*i+1] = m.getChLeftUp();
	}

	// return all channel numbers that belong to any of the modules
	return activeChannels;
}
