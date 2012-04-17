/*
 * CPositionXY.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CPositionXY.h"

CPositionXY::CPositionXY()
	: CPosition(2)
{
	mType = XY;
	mName = "XY";

	// Init the position variables, set them to be fixed.
	mParamNames.push_back("N");
	mParamNames.push_back("E");

	SetAllFree(false);
}

CPositionXY::~CPositionXY()
{
	// Do nothing
}


void CPositionXY::GetXYZ(double & x, double & y, double & z)
{
	x = mParams[0];
	y = mParams[1];

	// By default
	z = 0;
}
