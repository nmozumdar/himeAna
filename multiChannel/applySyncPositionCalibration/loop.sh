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
# select the data you want to calibrate
subdir=2026-04-21
#subdir=2024-06-25_cosmics
geometryFile=2026-05-04.csv
# The thresholds will NOT apply a cut on the data that are written to the output TTree object!
# Only the data shown in the histograms are affected.
# Leave empty (also remove spaces or tab symbols behind "=") to apply no thresholds.
thresholdsFile=2024-06-21.csv
# here you define the (previously determined) position calibration that you want to apply
subdir_calibration=2026-04-21
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then
	
	fileCounter=0
	
	create_directory applyPositionCalibration "$subdir"

	for filename in "$HIME_DATA_DIRECTORY"/data/tDiff/"$subdir"/*.root; do
		check_threads "$fileCounter"
		filename=$(basename "$filename")
		$ROOT_CALL "applyCalibration( \"${HIME_DATA_DIRECTORY}\", \"${subdir}\", \"${filename}\", \"${geometryFile}\", \"${thresholdsFile}\",\"${subdir_calibration}\", false)" > /dev/null &
		fileCounter=`expr ${fileCounter} + 1`
	done

	wait
	echo -e "\nloop.sh done."
fi;
