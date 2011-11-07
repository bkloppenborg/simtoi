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
	this->n_params_free = 0;

}

CPositionXY::~CPositionXY()
{
	// Do nothing
}

void CPositionXY::SetParams(double * params)
{
	// does nothing for now.
}

void CPositionXY::GetXYZ(double & x, double & y, double & z)
{
	x = 0;
	y = 0;

	// By default
	z = 0;
}
