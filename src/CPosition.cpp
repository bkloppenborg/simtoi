/*
 * CPosition.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CPosition.h"
#include "misc.h"	// needed for pull_params

CPosition::CPosition()
{
	// TODO Auto-generated constructor stub

}

CPosition::~CPosition()
{
	delete params;
	delete free_params;
}

// Initializes free space for holding the (fixed and free) parameter values.
void CPosition::init_params(int total_params)
{
	params = new float[total_params];
	free_params = new bool[total_params];

	for(int i = 0; i < total_params; i++)
	{
		params[i] = 0;
		free_params[i] = true;
	}
}

int CPosition::GetNFreeParameters()
{
	return this->n_params_free;
}

void CPosition::GetParams(float * out_params, int n_params)
{
	pull_params(this->params, n_params_total, out_params, n_params, free_params);
}

ePositionTypes CPosition::GetType()
{
	return this->type;
}

/// Sets the input parameters given the free parameter mask.
void CPosition::SetParams(float * in_params, int n_params)
{
	// TODO: Check that n_params > this->n_free_params, if not throw exception
	pull_params(in_params, n_params, this->params, n_params_total, free_params);
}
