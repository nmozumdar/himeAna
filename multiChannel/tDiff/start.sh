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
#subdir=2025-12-10
subdir=2026-04-22
#filename=himede_layer11_modules16-23_900V_0x200_0000.root
filename=samurai_2_1T_Y_source26112094105.root
# select the CSV file where the channel mapping is defined
#channelMapping=2024-09-03_de.csv
trigger=-1
# select the CSV file where the channel mapping is defined
channelMapping=2026-04-21.csv
#channelMapping=2025-10-03.csv
# choose if you want to take only the first pulse in each bar
# for your analysis. Otherwise, multiple hits can be detected 
# in each module, but there might be more noise.
# false -> first hit only;   true -> all hits
multihit=false
plot=true
write=true
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	create_directory tDiff "$subdir"

	# start 
	filename=$(basename "$filename")
	#$ROOT_CALL "tDiff(\"${HIME_ANA_DIRECTORY}\",\"${subdir}\",\"${filename}\",\"${channelMapping}\",${trigger},${multihit},${write},${plot})" 
	$ROOT_CALL "tDiff(\"${HIME_DATA_DIRECTORY}\",\"${subdir}\",\"${filename}\",\"${channelMapping}\",${trigger},${multihit},${write},${plot})" 

	wait
	echo -e "\nstart.sh done."
fi;
