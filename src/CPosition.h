/*
 * CPosition.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A class serving as a base-class and lookup class for positions.
 *  If you add a new position type, create
 */

#ifndef CPOSITION_H_
#define CPOSITION_H_

#include "CParameters.h"
#include <vector>
#include <utility>

class CPosition : public CParameters
{
public:
	enum PositionTypes
	{
		NONE,
		XY,
		XYZ,
		ORBIT,
		LAST_VALUE
	};

protected:
	CPosition::PositionTypes mType;

public:
	CPosition(int n_parameters);
	virtual ~CPosition();

	CPosition::PositionTypes GetType() { return mType; };

	// Computes the (X,Y,Z) position of an object.  Z should be set to zero if not computed.
	virtual void GetXYZ(double & x, double & y, double & z);
	static CPosition * GetPosition(CPosition::PositionTypes type);
	static vector< pair<CPosition::PositionTypes, string> > GetTypes();
};

#endif /* CPOSITION_H_ */
