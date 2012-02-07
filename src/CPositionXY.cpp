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
	mParamNames.push_back("X");
	mParamNames.push_back("Y");

	SetAllFree(false);
}

CPositionXY::~CPositionXY()
{
	// Do nothing
}


void CPositionXY::GetXYZ(float & x, float & y, float & z)
{
	x = mParams[0];
	y = mParams[1];

	// By default
	z = 0;
}
