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

#ifndef Track_h
#define Track_h

#include <vector>
#include "TGraph.h"
#include "TF1.h"

class Track{
	
	public:

	Track();

	// remove all hits from the track
	void clear();

	// add the position of a hit in this track,
	// represented by a 2-dimensional point (y, x_or_z)
	void addHit(int hitID, float x_or_z, float y);

	// fit linear function to the 2-dimensional points (y, x_or_z)
	void fit();

	// remove the hit with the largest deviation to the muon track
	void removeHitWithLargestDev();

	// calculate the angle of incidence in degree (measured relative to the ground; i.e. most muons have angles around 90 deg)
	// of a muon track that is given as a function x(y)
	float calculateAngleOfIncidence();

	// Get the largest deviation of all hits to the muon track (x-ccordinate difference).
	// LayerTrack::fit() needs to be invoked before; otherwise, -1 is returned.
	inline float getMaxDev() const { return maxDev; }

	// get the current number of hits in this layer
	inline int getNHits() const { return hitGraph.GetN(); }

	// get the ID numbers of all hits in this layer
	inline std::vector<int> getHitIDs() const { return hitIDs; }

	// get the reconstructed muon track
	inline TF1 getMuonTrack() const { return *muonTrack; }

	// get the slope of the fit function x(y) (note: it's x as a function of y, not vice versa)
	inline float getSlope() const { return muonTrack->GetParameter(1); }

	// get the deviation of each hit to the fit function 
	inline std::vector<float> getDevs() const  { return allDevs; }

	// draw the TGraph of hits and the reconstructed muon track
	void draw();
	// hits of this track
	TGraph hitGraph;

	protected:

	std::vector<float> allDevs;
	float maxDev;
	int hitWithMaxDev;


	// reconstructed muon track
	TF1* muonTrack;

	// The hit numbers in the "TGraph hitGraph" run from 0 to "hitGraph.GetN()",
	// but these numbers don't allow for correlations with the ToT.
	// In order to correlate the hits in this layer with the corresponding ToT values,
	// the hit IDs are stored here.
	std::vector<int> hitIDs;
};

#endif