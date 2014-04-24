/*
 * CUniformSpot.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: bkloppenborg
 */

#include "CUniformSpot.h"

CUniformSpot::CUniformSpot():
	CFeature(3)
{
	mParamNames.push_back("theta");
	SetParam(0, 3.0);
	SetFree(0, true);
	SetMax(0, 6.0);
	SetMin(0, 0.1);

	mParamNames.push_back("phi");
	SetParam(1, 3.0);
	SetFree(1, true);
	SetMax(1, 6.0);
	SetMin(1, 0.1);

	mParamNames.push_back("radius");
	SetParam(2, 3.0);
	SetFree(2, true);
	SetMax(2, 6.0);
	SetMin(2, 0.1);

}

CUniformSpot::~CUniformSpot()
{
	// TODO Auto-generated destructor stub
}

