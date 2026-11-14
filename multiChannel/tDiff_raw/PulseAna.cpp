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

#include "PulseAna.h"
using std::array;
using std::vector;
using MF = hadaq::MessageFloat;

bool PulseAna::findPulse(vector<MF*>& messages, array<float,2>& timeStamps){
	if(messages.size() < 2) return false;
	
	// iterate over MF objects that are ordered in time
	for(int iMsg = 0; iMsg < messages.size() - 1; iMsg++){
		// look at pairs of subsequent MF objects
		MF& mf_first = *(messages[iMsg]);
		MF& mf_second = *(messages[iMsg+1]);

		// if the first MF is rising and the second one falling,
		// we have a complete pulse
		// -> return true
		if(mf_first.isRising() && mf_second.isFalling()){
			timeStamps = {mf_first.getStamp(), mf_second.getStamp()};
			return true;
		}
		// if there are two rising or two falling edges,
		// a signal got lost
		// -> return false, because we don't know what happened exactly
		if(mf_first.isRising() && mf_second.isRising()){
			return false;
		}
		if(mf_first.isFalling() && mf_second.isFalling()){
			return false;
		}
	}
	// there might be just one signal that was detected successfully
	// -> in this case, a signal got lost and we cannot calculate the ToT
	// -> return false
	return false;
}



bool PulseAna::findPulses(vector<MF*>& messages, vector<array<float,2>>& timeStamps){
	if(messages.size() < 2) return false;
	
	// this variable tells if at least one rising edge was found, 
	// which was followed by a falling edge
	bool pulseFound = false;

	// iterate over MF objects that are ordered in time
	for(int iMsg = 0; iMsg < messages.size() - 1; iMsg++){
		// look at pairs of subsequent MF objects
		MF& mf_first = *(messages[iMsg]);
		MF& mf_second = *(messages[iMsg+1]);

		// if the first MF is rising and the second one falling,
		// we have a complete pulse
		// -> return true
		if(mf_first.isRising() && mf_second.isFalling()){
			timeStamps.push_back({mf_first.getStamp(), mf_second.getStamp()});
			pulseFound = true;
		}
		// if there are two rising or two falling edges,
		// a signal got lost
		// -> return false, because we don't know what happened exactly
		if(mf_first.isRising() && mf_second.isRising()){
			return false;
		}
		if(mf_first.isFalling() && mf_second.isFalling()){
			return false;
		}
	}
	
	return pulseFound;
}
