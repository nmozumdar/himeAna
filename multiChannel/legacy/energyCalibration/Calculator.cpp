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

#include "Calculator.h"



void Calculator::relativeResolution(Module& m, double energy_at_which_to_calculate_dEPerE){
	double tot = findRoot(m.calibrationFunction, energy_at_which_to_calculate_dEPerE);
	int totBin = m.hEDepVsTot.GetXaxis()->FindBin(tot);
	double sigma = m.hEDepVsTot.ProjectionY("", totBin, totBin)->GetStdDev();
	double relUnc = sigma / energy_at_which_to_calculate_dEPerE;
	m.relativeResolution = relUnc;
}



double Calculator::findRoot(const TF1*f, double e){

	const double *par = f->GetParameters();
	
	// solve linear equation
	if(f->GetNpar() == 2){
		return (e - par[0]) / par[1];
	}

	// solve quadratic equation
	return (-par[1] + std::sqrt(par[1] * par[1] - 4 * par[2] * (par[0]-e))) / (2. * par[2]);
}