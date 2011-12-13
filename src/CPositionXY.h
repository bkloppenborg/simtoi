/*
 * CPositionXY.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 */

#ifndef CPOSITIONXY_H_
#define CPOSITIONXY_H_

#include "CPosition.h"

class CPositionXY: public CPosition
{
public:
	CPositionXY();
	virtual ~CPositionXY();

	virtual void GetXYZ(float & x, float & y, float & z);
};

#endif /* CPOSITIONXY_H_ */
