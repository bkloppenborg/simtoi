/*
 * CPosition.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#include "CPosition.h"

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
	params = new double[total_params];
	free_params = new bool[total_params];

	for(int i = 0; i < total_params; i++)
	{
		params[i] = 0;
		free_params[i] = false;
	}
}

int CPosition::GetNFreeParameters()
{
	return this->n_params_free;
}

ePositionTypes CPosition::GetType()
{
	return this->type;
}
