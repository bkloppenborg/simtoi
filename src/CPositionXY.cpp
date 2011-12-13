/*
 * CPositionXY.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CPositionXY.h"

CPositionXY::CPositionXY()
{
	// Set the datamembers:
	this->n_params_total = 2;
	this->n_params_free = 2;

	init_params(n_params_total);
}

CPositionXY::~CPositionXY()
{
	// Do nothing
}


void CPositionXY::GetXYZ(float & x, float & y, float & z)
{
	x = params[0];
	y = params[1];

	// By default
	z = 0;
}
