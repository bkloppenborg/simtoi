/*
 * CPositionXY.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CPositionXY.h"
#include "misc.h"	// needed for pull_params

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

void CPositionXY::SetParams(double * in_params, int n_params)
{
	// TODO: Check that n_params > this->n_free_params, if not throw exception

	pull_params(in_params, n_params, this->params, n_params_total, free_params);
}

void CPositionXY::GetXYZ(double & x, double & y, double & z)
{
	x = params[0];
	y = params[1];

	// By default
	z = 0;
}
