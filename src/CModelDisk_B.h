/*
 * CModelDisk_B.h
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 *
 *  Disk Model A:
 *  This model modifies the edges of the simple CModelDisk to extend radially outward
 *  with an decaying hight that follows a gaussian:
 *
 *  	h(r) = h0 * exp( -(r-r0)^2/(2*h_r^2 )
 *
 *  where h0 is the initial height, r0 is the inner radius of the decaying region and
 *  h_r is a decay factor (akin to scale height)
 *
 */

#ifndef CMODELDISK_B_H_
#define CMODELDISK_B_H_

#include "CModelDisk.h"

class CModelDisk_B: public CModelDisk
{
public:
	CModelDisk_B();
	virtual ~CModelDisk_B();

	double GetRadius(double half_height, double h, double dh, double rim_radius);
};

#endif /* CMODELDISK_B_H_ */
