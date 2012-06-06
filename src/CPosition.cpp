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
#include "CPositionXY.h"
#include "CPositionOrbit.h"

CPosition::CPosition(int n_parameters)
	: CParameters(n_parameters)
{
	// Init to an invalid position type.
	mType = NONE;
}

CPosition::~CPosition()
{

}

CPosition * CPosition::GetPosition(CPosition::PositionTypes type)
{
	CPosition * tmp = NULL;

	// Otherwise assign the position.
	switch(type)
	{
	case ORBIT:
		tmp = new CPositionOrbit();
		break;

	case XY:
	default:
		// By default models use XY position.
		tmp = new CPositionXY();
		break;
	}

	return tmp;
}

/// Returns a pair of model names, and their enumerated types
vector< pair<CPosition::PositionTypes, string> > CPosition::GetTypes(void)
{
	vector< pair<CPosition::PositionTypes, string> > tmp;
	tmp.push_back(pair<CPosition::PositionTypes, string> (CPosition::XY, "XY"));
	tmp.push_back(pair<CPosition::PositionTypes, string> (CPosition::ORBIT, "Orbit"));

	return tmp;
}

/// Baseclass function, set all zeros
void CPosition::GetXYZ(double & x, double & y, double & z)
{
	x = 0;
	y = 0;
	z = 0;
}
