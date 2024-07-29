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
subdir=2022-11-30b/
# ------------------------------

source ../../common/common.sh

make

if [ $? -eq 0 ]; then

	# create a list of all root files in subdir and write it to tmpfile.txt~
	write_filenames_to_tmpfile tDiff "$subdir"

	# create directory for output
	create_directory trackingForPositionCalibration "$subdir"

	# start 
	$ROOT_CALL "tracking(\"${HIME_ANA_DIRECTORY}\",\"${subdir}\")"

	rm -f tmpfile.txt~
	
	wait
	echo -e "\nstart.sh done."
fi;
