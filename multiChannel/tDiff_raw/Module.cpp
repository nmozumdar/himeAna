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

#include "Module.h"
#include "Convert.h"
#include "TMath.h"



Module::Module(){
	id = -1;
	ch_left_up = -1;
	ch_right_down = -1;
}



Module::Module(int id, int ch_left_up, int ch_right_down){
	this->ch_left_up = ch_left_up;
	this->ch_right_down = ch_right_down;
	this->id = id;
	
	TString id_str = Convert::toNdigit(id, 3);
	TString ch_left_up_str = Convert::toNdigit(ch_left_up, 3);
	TString ch_right_down_str = Convert::toNdigit(ch_right_down, 3);
	TString channelInfoInParenthesis(" (left/top channel: " + ch_left_up_str + ", right/bottom channel: " + ch_right_down_str + ")");
	TString moduleInfoInParenthesis(" (module " + id_str + ")");

	hTotVsTDiff = TH2F("hTotVsTDiff_module_" + id_str, "Module " + id_str + channelInfoInParenthesis, 640, -40, 40, 100, 0, 50);
	hTot_left_up = TH1F("hTot_ch_" + ch_left_up_str, "Channel " + ch_left_up_str + moduleInfoInParenthesis, 100, 0, 50);
	hTot_right_down = TH1F("hTot_ch_" + ch_right_down_str, "Channel " + ch_right_down_str + moduleInfoInParenthesis, 100, 0, 50);

	hTotVsTDiff.GetXaxis()->SetTitle("t_{" + ch_left_up_str + "} - t_{" + ch_right_down_str + "} \\; \\mbox{(ns)}");
	hTotVsTDiff.GetYaxis()->SetTitle("\\sqrt{\\mbox{ToT}_{" + ch_left_up_str + "} \\cdot \\mbox{ToT}_{" + ch_right_down_str + "}} \\; \\mbox{(ns)}");
	hTot_left_up.GetXaxis()->SetTitle("ToT (ns)");
	hTot_right_down.GetXaxis()->SetTitle("ToT (ns)");
}



void Module::fillHistograms(float tDiff, float tot_left_up, float tot_right_down){
	hTotVsTDiff.Fill(tDiff, TMath::Sqrt(tot_left_up * tot_right_down));
	hTot_left_up.Fill(tot_left_up);
	hTot_right_down.Fill(tot_right_down);
}



void Module::write(TFile* f) const {
	f->cd();
	hTotVsTDiff.Write();
	hTot_left_up.Write();
	hTot_right_down.Write();
}