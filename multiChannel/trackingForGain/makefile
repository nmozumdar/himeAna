
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

headers = $(wildcard *.h)
sources = $(wildcard *.cpp)

.PHONY: clean

libTracking.so: $(sources) $(headers)
	g++ -shared -fPIC -O2 -o $@ $(sources) $(shell root-config --cflags --libs) -I. -I../../common/hime -L../../common/lib -lCommon

clean:
	@rm -f libTracking.so