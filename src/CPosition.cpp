/*
 * CPosition.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
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
