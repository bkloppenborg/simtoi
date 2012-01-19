/*
 * CPositionXYZ.h
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 *
 *  An position class implementing 3D positioning.
 */

#ifndef CPOSITIONXYZ_H_
#define CPOSITIONXYZ_H_

#include "CPosition.h"

class CPositionXYZ: public CPosition
{
public:
	CPositionXYZ();
	virtual ~CPositionXYZ();

	void GetXYZ(float & x, float & y, float & z);
};

#endif /* CPOSITIONXYZ_H_ */
