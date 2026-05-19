#!/bin/bash

#	HIMEana: Analyze HIME data.
#	
#	Copyright (C) 2023, 2024 Marco Knösel (mknoesel@ikp.tu-darmstadt.de)
#
#	This file is part of HIMEana.
#	
#	HIMEana is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
#	(at your option) any later version.
#
#	HIMEana is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#
#	You should have received a copy of the GNU General Public License
#	along with HIMEana.  If not, see <https://www.gnu.org/licenses/>.

# ---------- settings ----------
tDiffDir=run_237
tDiffFilename=run_237.root
mainDAQDir=/d/d04-1/ag_au/HIME_2022/unpacked/de/test_mainDAQ
mainDAQFilename=run0237.root
outputDir=syncTSWithSamurai
outputFilename="${mainDAQFilename%.root}_synced.root"

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	create_directory "syncTSWithSamurai" "$outputDir"

	# start 
	$ROOT_CALL "syncTSWithSamurai( \"${HIME_DATA_DIRECTORY}\", \"${tDiffDir}\", \"${tDiffFilename}\", \"${mainDAQDir}\", \"${mainDAQFilename}\", \"${outputDir}\", \"${outputFilename}\")"

	wait
	echo -e "\nstart.sh done."
fi;