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

#include "Drawer.h"
#include "Convert.h"
#include "TCanvas.h"
#include "TText.h"
#include "Helpers.h"
#include "Constants.h"
#include <vector>
using std::vector;



void Drawer::drawOverview(const HistogramCollection& hc){
	TCanvas* can = new TCanvas("canOverview", "", 1200, 900);
	can->Divide(2,1);
	can->cd(1);
	gPad->SetTicks();
	hc.hPos.DrawClone("colz");
	can->cd(2);
	gPad->SetTicks();
	hc.hZ.DrawClone();
}



void Drawer::drawLayer(const HistogramCollection& hc, int layer, const vector<Module>& modules){

	if( !hc.hPosLayer[layer].GetEntries() ) return;

	TCanvas* can = new TCanvas("can_layer_" + Convert::toStr(layer), "Layer " + Convert::toStr(layer), 1200, 900);
	hc.hPosLayer[layer].DrawClone("colz");

	vector<TText*> labels(Constants::nModules);
	for(const Module& m : modules){
		if(m.id < 0) continue;
		if( layer != Helpers::getLayer(m.id) ) continue;
		labels[m.id] = new TText(m.x, m.y, Convert::toStr(m.id));
		labels[m.id]->SetTextSize(0.01);
		labels[m.id]->SetTextAlign(22);
		labels[m.id]->Draw();
	}
}