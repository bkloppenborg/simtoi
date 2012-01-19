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
