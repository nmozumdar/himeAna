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
subdir=2024-07-10_cosmics
thresholds=2024-07-26.csv
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	create_directory simplePositionCalibration "$subdir"

	write_fileNames_to_tmpfile tDiff "$subdir"

	# calculate calibration functions
	$ROOT_CALL "calculateCalibrationFunctions(\"${HIME_ANA_DIRECTORY}\",\"${subdir}\",\"${thresholds}\")"

	rm -f tmpfile.txt~
	
	# print message when all jobs are completed
	wait
	echo -e "\nstart.sh done."
fi


