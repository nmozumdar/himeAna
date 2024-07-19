# HIMEana - Analyze HIME Data

## Copyright Notice and License
HIMEana serves to analyze data from the Neutron Detector 
"HIgh-resolution detector array for Multi-neutron Events" (HIME).

Copyright (C) 2023, 2024 Marco Knösel (mknoesel@ikp.tu-darmstadt.de)

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

---

## Contents
- [HIMEana - Analyze HIME Data](#himeana---analyze-hime-data)
	- [Copyright Notice and License](#copyright-notice-and-license)
	- [Contents](#contents)
	- [Initial Setup](#initial-setup)
	- [HIME Nomenclature and Unique Channel Numbering](#hime-nomenclature-and-unique-channel-numbering)
	- [Structure of the HIMEana Tools](#structure-of-the-himeana-tools)
		- [Configure Details about the Setup of HIME](#configure-details-about-the-setup-of-hime)
		- [Steps of the multiChannel Analysis](#steps-of-the-multichannel-analysis)
		- [Common Source Code](#common-source-code)
	- [Compile and Start](#compile-and-start)
	- [Files taken from the Stream framework](#files-taken-from-the-stream-framework)
		- [Copyright Notice and License for the Stream Framework](#copyright-notice-and-license-for-the-stream-framework)

## Initial Setup
1. Go to directory `data` and create a symbolic link called `unpacked` that points towards your unpacked data:
    `ln -s [path] unpacked`.
    Inside `unpacked`, create a directory for each **experiment**, in which arbitrarily many ROOT files can be located, each one representing a **run**.
2. Modify the settings section in the file `common/common.sh`. (It is **not** required to execute `common/common.sh`. Instead, it will be sourced by other shell scripts.) 
   1. Choose the root version you want to source.
   2. (optional) Modify the command that is used to call ROOT. This might be useful if you are using an older version of ROOT that does not yet include a web server to display all your instances of TCanvas, TBrowser, etc. in your web browser.
   3. Enter the complete absolute path to your `himeAna` directory.
3. In the same directory `common`, type `./compileCommons.sh`. This will create shared libraries that are loaded in each of the individual analysis steps.

## HIME Nomenclature and Unique Channel Numbering
- Module: The combination of a scintillator and it's two photomultiplier tubes (PMTs)
- Layer: An array of 24 either horizontally or vertically aligned modules
- Detector wall: A set of layers

The analogue signals that come from the photomultipliert tubes (PMTs) are discriminated by the PaDiWa boards. Each of them has 16 channels in is connected via a DAC chain to a peripheral field programmable gate array (FPGA). There can be up to 3 PaDiWa boards for an FPGA, so in total, each FPGA can cover 48 channels. The FPGAs serve as time-to-digital converters (Not all of them, but all where PaDiWa boards are connected). On each TRB3 board, we can use up to 4 FPGAs as TDCs, so up to 192 channels can be read out. 

In order to have a unique number for each channel, the HIME channel number is defined as 

	HIME channel = (DAC chain) x 16 + (FPGA number) x 48 

This will be extended by `... + (TRB3 number) x 192` once we have more TRB3 boards in operation.

The numbering will always start from 0: The first layer is layer 0, the first HIME channel is 0, the first detector wall is wall 0, the first PaDiWa channel is 0, ...

## Structure of the HIMEana Tools
The two main parts of HIMEana are `fourChannel` and `multiChannel`. The former is used to have a look on uncalibrated data from up to 4 channels and allows to cut on coincidences between two modules. The latter is used to calibrate all modules of HIME in time, position and energy, which is done is multiple steps. For each step, there is a subdirectory in `multichannel`, which reads and writes from/to the corresponding subdirectories of `data`. 

To some of the subdirectories of `data`, CSV files have to be added manually. See [Configure Details about the Setup of HIME](#configure-details-about-the-setup-of-hime).

The reason for the full analysis being divided in small steps is that the output should be checked carefully after each part of the calibration. Besides, there are two different approaches available for the position calibration.

### Configure Details about the Setup of HIME
In the directories `data/thresholds`, `data/geometry/` and `data/channelMapping/`, CSV files have to be added manually in order to correctly configure the calibration procedure:
1. `data/channelMapping/`: Tells which channels belong to the same module.
2. `data/thresholds`: ToT values for all modules above which the muon hits are located. Used to cut away low-energetic terrestrial background.
3. `data/geometry/`: Lists the positions of all modules.

### Steps of the multiChannel Analysis
In the following, the individual parts of the multiChannel analysis are listed. They use data from each other, shared in the directory `data`, and thus should be applied in the right order.

1. `tDiff`: The bare unpacked data contain time stamps for the rising and falling edges of all signals in each channel, as well as the number of the trigger signal of each event. In this analysis step, the ToT values and the time differences of detection in the two PMTs of each module are calculated from the sequences of signals. Besides, one can select data from a specific trigger type. In order to assign channel numbers to module IDs (two channels for each module), one can either use a CSV file in `data/channelMapping/` or a hard-coded definition in one of the overloaded constructers of class `Detector`. Make sure to call the right constructor in `multichannel/tDiff/trb3Ana.cpp`.
2. Position calibration (two choices):
   - either simple: 
     `simplePositionCalibration`: Treats each module individually and calculates each linear calibration function from two data pairs (consisting of time difference and position) only. Rather simple and robust method.
   - or based on muon tracking:
     1. `trackingForPositionCalibration`: Tracks cosmic muons through the detector and writes down correlation plots of time differences (i.e., time difference of detection of the signals of the two PMTs of each module) and muon position for each module. This is a rather complicated method, so its results should be checked carefully. It gets more precise with a larger number of layers and modules.
     2. `positionCalibrationFromTracking`: Fits linear calibration functions to the previously determined correlation plots.
3. `applyPositionCalibration`: Applies the position calibration on the data.
4. Tracks again muons in HIME, but makes use of the position calibration. The results are correlation plots of ToT and muon-energy deposition. The latter is calculated from the track lengths of the cosmic particles inside each module.
   - either simple with 2D tracks: `simpleTrackingForEnergyCalibration`: works with only 3 layers
   - or more complex with 3D tracks: `trackingForEnergyCalibration`: needs at least 4 layers; less reliable with a small number of layers
5. `energyCalibration`: Fits calibration functions to the previously determined correlation plots. (Not yet tested with `simpleTrackingForEnergyCalibration`)

Besides, there is a tool called `sumUp` in `multiChannel`, which serves to sum up spectra and correlation plots from all runs of the same experiment. It is not a part of the calibration procedure, but can be used for visualization after some of the individual analysis steps.

### Common Source Code
Some of the functionality of HIMEana is shared between its individual analysis steps: For instance, the reader for CSV files is required in multiple components of `multiChannel`. Usually, it will not be necessary to modify the corresponding source code, but if necessary, this can be done in the subdirectories of `common`. After the modifications are done, recompile by going to `common` and executing `compileCommons.sh`.

## Compile and Start
For each component (`fourChannel` and each step of `multiChannel`) of HIMEana, there is at least one shell script that is used to compile the source code and to start the analysis if the compilation was successful. These shell scripts are called `start.sh` and `loop.sh`:
- `start.sh` will start a single thread only. Some of the components of `multiChannel` are not suitable for parallelization. If so, you can specify in `start.sh` which experiment you want to analyze and the program will iterate over all runs. Otherwise, you can specify which run of that experiment you want to analyze. Consequently, only a data from a single file will be taken into account.
- `loop.sh` will start a number of threads in parallel. The amount of threads can be specified in `thread_goal.txt`. Having this number in a separate text file (instead of having it hard coded in the shell script) allows to change the desired number of threads even after `loop.sh` has been started. In the shell script, you need to specify which experiment you want to look at. `loop.sh` will only finish after **all** runs have been analyzed.

Always `cd` at first to the directory of the analysis step you want to perform before executing `start.sh` or `loop.sh`.

If you only want to compile a component of HIMEana, without starting it directly, simply type `make` in the corresponding directory. In order to compile from scratch, type `make clean` before. Starting an analysis should **always** be done using one of the shell scripts! 

Note that the source code in `common` needs to be compiled separately, i.e., it is **not** sufficient to compile `fourChannel` or one of the `multiChannel` components, if code inside `common` is modified.

## Files taken from the Stream framework
HIMEana contains the following files that were taken from the Stream framework and modified:
- `common/stream/HldProcessor.h`
- `common/stream/ScalerSubEvent.h`
- `common/stream/SubEvent.h`
- `common/stream/TdcSubEvent.h`
- `common/stream/TrbProcessor.h`

### Copyright Notice and License for the Stream Framework

Copyright (C) 2013 -
GSI Helmholtzzentrum fuer Schwerionenforschung GmbH
Planckstr. 1, 64291 Darmstadt, Germany
Contact:  https://github.com/gsi-ee/stream

Modifications Copyright (C) 2023 Marco Knösel (mknoesel@ikp.tu-darmstadt.de)

The Stream framework is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3
of the License, or (at your option) any later version.

The Stream framework is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the Stream framework. If not, see <https://www.gnu.org/licenses/>.