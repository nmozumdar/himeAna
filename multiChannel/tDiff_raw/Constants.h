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

#ifndef Constants_h
#define Constants_h


namespace Constants{
	// number of peripheral TDCs
	const int nTdcs = 16;
	// number of channels per TDC
	const int nChPerTdc = 48;
	// total number of channels
	const int nChTot = nTdcs * nChPerTdc;
	// total number of modules of the detector
	const int nModules = 384;
	// hime+ cabling is not sequential
	const int TDCoff[nTdcs]={1,3,5,6,7,9,10,11,0,2,4,8,12,13,14,15};
};

#endif
