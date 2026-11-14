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

#ifndef PulseAna_h
#define PulseAna_h

#include "TdcSubEvent.h"
#include <array>

namespace PulseAna{
	/*
		Find a rising edge that is followed by a falling edge and save the respective
		time stamps in a std::array.
		Return true if such a sequence of messages is found;
		return false otherwise.
	*/
	bool findPulse(std::vector<hadaq::MessageFloat*>& messages, std::array<float,2>& timeStamps);
	bool findPulses(std::vector<hadaq::MessageFloat*>& messages, std::vector<std::array<float,2>>& timeStamps);
};

#endif