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

#ifndef Fit_h
#define Fit_h

#include "Track.h"
#include "Detector.h"

namespace Fit{
	/*
		Functions to fit sets of (x,z) and (y,z) data pairs
	*/
	bool fitLoopForTracking(Track &t, const Detector &d);
	void fit(Track &t);
	bool checkDeviation(Track &t);
	float calculateDeviation(int point, const Track &t);
	bool hitsInAtLeastThreeLayers(Track &t, const Detector &d);
};

#endif