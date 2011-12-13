/*
 * CPosition.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A purely virtual class that computes the (X,Y,0) or (X,Y,Z) position of a
 *  particular model object.
 */

#ifndef CPOSITION_H_
#define CPOSITION_H_

#include "enumerations.h"

class CPosition
{
protected:
	int n_params_total;
	int n_params_free;

	ePositionTypes type;

	float * params;
	bool * free_params;

	void init_params(int total_params);

public:
	CPosition();
	virtual ~CPosition();

	int GetNFreeParameters();
	ePositionTypes GetType();

	// Gets the current position for the object, after the parameters have been set and scaling has been applied.
	void GetParams(float * in_params, int n_params);

	// Computes the (X,Y,Z) position of an object.  Z should be set to zero if not computed.
	virtual void GetXYZ(float & x, float & y, float & z) = 0;

	// A function to route and scale unit hypercube variables to object parameters.
	void SetParams(float * in_params, int n_params);


};

#endif /* CPOSITION_H_ */
