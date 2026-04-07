#!/bin/bash

# ---------- settings ----------
#source /home/hime/trbsoft/trb3/rootbuild/bin/thisroot.sh
export ROOT_CALL="root -l --web=server:8877 -e"
export HIME_ANA_DIRECTORY="/home/hime/himeAna"
export HIME_DATA_DIRECTORY="/d/d04-1/ag_au/HIME_2022"
# ------------------------------

export LD_LIBRARY_PATH=$HIME_ANA_DIRECTORY/common/lib:$LD_LIBRARY_PATH

# check the amount of started threads
check_threads () {
	REMAINING_JOBS=(`jobs -p`)
	THREAD_GOAL=`cat thread_goal.txt`
	while [ ${#REMAINING_JOBS[@]} -ge $THREAD_GOAL ]; do
		echo -n -e "[common.sh] ${#REMAINING_JOBS[@]} jobs running, goal is ${THREAD_GOAL}. Waiting before starting more. Already started: $1\r"
		sleep 5
		THREAD_GOAL=`cat thread_goal.txt`
		REMAINING_JOBS=(`jobs -p`)
	done
}

# create a new directory for output 
create_directory () {
	OUTPUT_DIRECTORY="$HIME_DATA_DIRECTORY"/data/"$1"/"$2"
	if [ ! -d "$OUTPUT_DIRECTORY" ]; then
		echo -e "\n"[common.sh] creating directory "$OUTPUT_DIRECTORY" "\n"
		mkdir "$OUTPUT_DIRECTORY"
	fi
}

# create a list of all root files in subdir
get_all_files () {
	ALL_FILES="{"
	for COMPLETE_PATH in "$HIME_ANA_DIRECTORY"/data/"$1"/"$2"/*.root; do
		ALL_FILES="$ALL_FILES\"$(basename $COMPLETE_PATH)\","
	done
	ALL_FILES=${ALL_FILES%?}
	ALL_FILES="$ALL_FILES""}"
}

# write the file names of all root files in subdir to a temporary file
write_filenames_to_tmpfile () {
	rm -f tmpfile.txt~
	for COMPLETE_PATH in "$HIME_DATA_DIRECTORY"/data/"$1"/"$2"/*.root; do
		echo $(basename $COMPLETE_PATH) >> tmpfile.txt~
	done
}
