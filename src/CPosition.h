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

class CPosition
{
protected:
	int n_params_total;
	int n_params_free;

public:
	CPosition();
	virtual ~CPosition();

	// A function to route and scale unit hypercube variables to object parameters.
	virtual void SetParams(double * params) = 0;

	// Computes the (X,Y,Z) position of an object.  Z should be set to zero if not computed.
	virtual void GetXYZ(double & x, double & y, double & z) = 0;
};

#endif /* CPOSITION_H_ */
