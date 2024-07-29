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

#include "Input.h"
#include <iostream>
#include <fstream>
using std::ifstream;
using std::string;
using std::cout;
using std::endl;
using std::vector;



Input::Input(TString directory){
	if( !directory.EndsWith("/") ) directory.Append("/");
	cout << "[Input] Reading directory " << directory.Data() << " ..." << endl;

	ifstream tmpfile("tmpfile.txt~", std::ios::in);	
	string filename;
	while(getline(tmpfile, filename)){
		filenames.push_back(filename);
	}
	tmpfile.close();

	this->directory = directory;
	files = vector<TFile*>(filenames.size());
}



TFile* Input::getFile(int run, bool verbose){
	if(verbose) cout << "[Input] Reading file " << filenames[run] << endl;
	files[run] = new TFile(directory + filenames[run], "read");
	return files[run];
}



void Input::closeFile(int run){
	files[run]->Close();
}