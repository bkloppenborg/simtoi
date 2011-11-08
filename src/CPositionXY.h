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

	virtual void SetParams(double * in_params, int n_params);
	virtual void GetXYZ(double & x, double & y, double & z);
};

#endif /* CPOSITIONXY_H_ */
