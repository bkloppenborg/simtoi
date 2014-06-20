/*
 * CPositionXY.cpp
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

#include "CPositionXY.h"

CPositionXY::CPositionXY()
	: CPosition(2)
{
	mName = "XY";
	mPositionID = "xy";

	// Init the position variables, set them to be fixed.
	mParamNames.push_back("N");
	SetParam(0, 0);
	SetFree(0, false);
	SetMax(0, 1);
	SetMin(0, -1);

	mParamNames.push_back("E");
	SetParam(1, 0);
	SetFree(1, false);
	SetMax(1, 1);
	SetMin(1, -1);

	SetAllFree(false);
}

CPositionXY::~CPositionXY()
{
	// Do nothing
}

CPositionPtr CPositionXY::Create()
{
	return CPositionPtr(new CPositionXY());
}

void CPositionXY::GetXYZ(double & x, double & y, double & z)
{
	// Astronomical convention has (North,East) = (Up,Left),
	// OpenGL has (x,y) = (right,up)
	// So we reassign (x,y) = (-East, North)

	x = -1 * mParams[1];
	y = mParams[0];

	// By default
	z = 0;
}
