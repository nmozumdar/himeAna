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
subdir=2024-06-21_shadow_w_and_wo_sbt_combined_manually
filename=hime_data_sbt_shadow_1039.root
geometryFile=2024-06-21.csv
thresholdsFile=2024-06-21.csv
subdir_calibration=2024-06-19_cosmics
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	create_directory applyPositionCalibration "$subdir"

	# start 
	filename=$(basename "$filename")
	$ROOT_CALL "applyCalibration( \"${HIME_ANA_DIRECTORY}\", \"${subdir}\", \"${filename}\", \"${geometryFile}\", \"${thresholdsFile}\",\"${subdir_calibration}\", true)"

	wait
	echo -e "\nstart.sh done."
fi;
