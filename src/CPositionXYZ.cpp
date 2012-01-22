/*
 * CPositionXYZ.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 */

#include "CPositionXYZ.h"


CPositionXYZ::CPositionXYZ()
	: CPosition(3)
{
	mType = POSITION_XYZ;
	mName = "XYZ";

	// Init the position variables, set them to be fixed.
	mParamNames.push_back("X");
	mParamNames.push_back("Y");
	mParamNames.push_back("Z");

	SetAllFree(false);
}

CPositionXYZ::~CPositionXYZ()
{
	// Do nothing
}


void CPositionXYZ::GetXYZ(float & x, float & y, float & z)
{
	x = mParams[0];
	y = mParams[1];
	z = mParams[2];
}
