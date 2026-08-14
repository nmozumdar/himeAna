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
#include "TAxis.h"
#include "Convert.h"

using std::array;



Module::Module(int id, int layer, bool horizontal, float x, float y, float z){

	// initialize TH2F for (tDiff, pos) data pairs
	hPosVsTDiff = new TH2F(TString("hPosVsTDiff_module_") + Convert::toNdigit(id, 3), "", 120, -60, 60, 100, -500, 500);
	//hPosVsTDiff = new TH2F(TString("hPosVsTDiff_module_") + Convert::toNdigit(id, 3), "", 200, -60, 60, 100, -500, 500);
	hDtNextBar = new TH1F(TString("hDtNextBar_module_") + Convert::toNdigit(id, 3), "", 200, -40, 40);
	hDtNextPlane = new TH2F(TString("hDtNextPlane_module_") + Convert::toNdigit(id, 3), "", 25, -0.5, 24.5, 200, -40, 40);

	// set title for TGraph and TH2F
	TString title("Module ");
	title += Convert::toStr(id);
	title += ", layer ";
	title += Convert::toStr(layer);
	title += horizontal ? " (horizontal)" : " (vertical)";
	hPosVsTDiff->SetTitle(title);

	// set axis titles
	hPosVsTDiff->GetXaxis()->SetTitle("\\Delta t \\mbox{ (ns)}");
	hPosVsTDiff->GetYaxis()->SetTitle((horizontal ? TString("x") : TString("y")) + " \\mbox{ (mm)}");

	avgToT = new TH1F(TString("AvgToT_module_") + Convert::toNdigit(id, 3), "", 200, 0, 50);
	avgToT->GetXaxis()->SetTitle("ToT (arb Units)");

	hEDepVsTot = new TH2F("hEDepVsTot_" + Convert::toNdigit(id, 3), title, 100, 0, 50, 160, 0, 40);
        hEDepVsTot->GetXaxis()->SetTitle("ToT (ns)");
        hEDepVsTot->GetYaxis()->SetTitle("Energy deposition (MeV)");


	hDtNextPlane->GetYaxis()->SetTitle("#Delta t (ns)");
	hDtNextPlane->GetXaxis()->SetTitle("Module");
	hDtNextBar->GetXaxis()->SetTitle("#Delta t (ns)");

	this->x = x;
	this->y = y;
	this->z = z;
	this->layer = layer;
	this->id = id;
}



void Module::write(TFile *file){
	file->cd();
	hPosVsTDiff->Write();
}
