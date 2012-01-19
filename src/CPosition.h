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

#include "CParameters.h"
#include "enumerations.h"

enum ePositionTypes
{
	POSITION_XY,
	POSITION_XYZ,
	POSITION_Orbit
};

class CPosition : public CParameters
{
protected:
	ePositionTypes mType;

public:
	CPosition(int n_parameters);
	virtual ~CPosition();

	ePositionTypes GetType() { return mType; };

	// Computes the (X,Y,Z) position of an object.  Z should be set to zero if not computed.
	virtual void GetXYZ(float & x, float & y, float & z) = 0;
};

#endif /* CPOSITION_H_ */
