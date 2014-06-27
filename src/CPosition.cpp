/*
 * CPosition.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CPosition.h"

CPosition::CPosition()
	: CParameterMap()
{
	id = "position_base_invalid";
	name = "not set by developer";
	mPositionType = STATIC;
	mTime = 0;
}

CPosition::~CPosition()
{

}

/// Returns the position of the object in OpenGL coordinates
///
/// OpenGL coordinates have (x,y,z) = (right, up, toward)
void CPosition::GetXYZ(double & x, double & y, double & z)
{
	x = 0;
	y = 0;
	z = 0;
}

void CPosition::SetTime(double time)
{
	mTime = time;
}
