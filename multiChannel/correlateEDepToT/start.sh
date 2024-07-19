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
subdir=2024-06-25_cosmics
filename=cosmics_layers_1_2_3_thr0x200_volt2024-06-25_0000.root
minMultiplicity=5
maxDev=70
allowedPositionRange=450
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	create_directory correlateEDepToT "$subdir"

	# start 
	filename=$(basename "$filename")
	$ROOT_CALL "correlateEDepToT( \"${HIME_ANA_DIRECTORY}\", \"${subdir}\", \"${filename}\", ${minMultiplicity}, ${maxDev}, ${allowedPositionRange})"

	wait
	echo -e "\nstart.sh done."
fi;
