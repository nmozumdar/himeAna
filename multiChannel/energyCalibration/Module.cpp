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
#include <iostream>
using std::cout;
using std::endl;



Module::Module(){
	maxGraph = TGraphErrors();
	iMod = -1;
}



void Module::addCorrelationPlot(const TH2F* h){
	if(hEDepVsTot.GetEntries()){
		hEDepVsTot.Add(h);
	}
	else{
		hEDepVsTot = TH2F(*h);
	}
}



void Module::setHorizontal(bool isHorizontal){
	this->isHorizontal = isHorizontal;
}



void Module::createCalibrationFunction(){

	//const char* formula = isHorizontal ? "[0] + [1] * x" : "[0] + [1] * x + [2] * x * x" ;
	const char* formula = "[0] + [1] * x";
	double rangeMin, rangeMax, tmp;
	maxGraph.GetPoint(0, rangeMin, tmp);
	maxGraph.GetPoint(maxGraph.GetN()-1, rangeMax, tmp);
	calibrationFunction = new TF1("calibrationFunction" + Convert::toStr(iMod), formula, rangeMin, rangeMax);
}



void Module::print(){
	cout << "\n*** Module " << iMod << " ***\n";
	cout << "$p_0 = \\SI{" << calibrationFunction->GetParameter(0) << "(" << calibrationFunction->GetParError(0) << ")}{\\mega\\electronvolt}$,\n";
	cout << "$p_1 = \\SI{" << calibrationFunction->GetParameter(1) << "(" << calibrationFunction->GetParError(1) << ")}{\\mega\\electronvolt\\per\\nano\\second}$ and\n";
	if(calibrationFunction->GetNpar() > 2){
		cout << "$p_2 = \\SI{" << calibrationFunction->GetParameter(2) << "(" << calibrationFunction->GetParError(2) << ")}{\\mega\\electronvolt\\per\\nano\\second\\squared}$.\n";
	}
	cout << "Relative resolution: " << 100. * relativeResolution << " %\n" << endl;
}
