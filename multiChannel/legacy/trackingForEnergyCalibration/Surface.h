/*
	HIMEana: Analyze HIME data.
	
	Copyright (C) 2023, 2024 Marco Knösel (mknoesel@ikp.tu-darmstadt.de)

	This file is part of HIMEana.
	
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
*/

#ifndef Surface_h
#define Surface_h

#include <array>

class Surface{
	public:
	Surface(){}
	Surface(std::array<std::array<float,2>,2> range, int orthogonalAxis, float posOnAxis);

	// axis that is orthogonal to the surface
	// 0 -> x, 1 -> y, 2->z
	int orthogonalAxis;

	// point where the plane (which the surface is contained in) crosses the orthogonal axis
	float posOnAxis;
	
	// spatial extension
	// if orthogonalAxis = 0: { {y_min, y_max},  {z_min, z_max} }
	// if orthogonalAxis = 1: { {x_min, x_max},  {z_min, z_max} }
	// if orthogonalAxis = 2: { {x_min, x_max},  {y_min, y_max} }
	std::array<std::array<float,2>,2> range;
};

#endif