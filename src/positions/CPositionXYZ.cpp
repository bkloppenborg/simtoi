/*
 * CPositionXYZ.cpp
 *
 *  Created on: Jan 19, 2012
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

#include "CPositionXYZ.h"

CPositionXYZ::CPositionXYZ()
{
	name = "XYZ";
	id = "xyz";
	mPositionType = STATIC;

	// Init the position variables, set them to be fixed.
	addParameter("N", 0, -1, 1, false, 0.1, "North", "North (mas) (positive is up on the screen)", 2);
	addParameter("E", 0, -1, 1, false, 0.1, "East", "East (mas) (positive is left on the screen)", 2);
	addParameter("Z", 0, -1, 1, false, 0.1, "Z", "Z-direction (mas) (positive is into of the screen)", 2);
}

CPositionXYZ::~CPositionXYZ()
{
	// Do nothing
}

CPositionPtr CPositionXYZ::Create()
{
	return CPositionPtr(new CPositionXYZ());
}

void CPositionXYZ::GetXYZ(double & x, double & y, double & z)
{
	// Astronomical convention has (North,East,z) = (Up,Left,away),
	// OpenGL has (x,y,z) = (right,up,towards)
	// So we reassign (x,y,z) = (-East, North,-z)

	x = -1 * mParams["E"].getValue();
	y = mParams["N"].getValue();
	z = -1 * mParams["Z"].getValue();
}
