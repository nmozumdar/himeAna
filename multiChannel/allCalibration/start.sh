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
subdir=2026-05-17
#subdir=test
# number of counts required in each projection in order to apply a Gaussian fit
nCountsPerProjection=40
# number of projections required in order to perform a fit of the linear position-calibration function
nProjections=2
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	# create directory for output
	create_directory CalibrationFromTracking "$subdir"
	
	# start 
	#$ROOT_CALL "calculateCalibrationFunctions(\"${HIME_ANA_DIRECTORY}\",\"${subdir}\",${nCountsPerProjection},${nProjections})" 
	$ROOT_CALL "calculateCalibrationFunctions(\"${HIME_DATA_DIRECTORY}\",\"${subdir}\",${nCountsPerProjection},${nProjections})" 

	wait
	echo -e "\nstart.sh done."
fi;
