/*
 * CFeature.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: bkloppenborg
 */

#include "CFeature.h"

CFeature::CFeature()
	: CParameterMap()
{
	mID = "invalid_feature";
	mName = "Not set by developer";
}

CFeature::~CFeature()
{
	// TODO Auto-generated destructor stub
}
