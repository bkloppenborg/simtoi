/*
 * CModelDisk_B.h
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 *
 *  Disk Model A:
 *  This model modifies the edges of the simple CModelDisk to extend radially outward
 *  with an decaying height that follows a power law:
 *
 *  	h(r) = h0 * (r_rim / r)^beta
 *
 *  where h0 is the initial height, r0 is the inner radius of the decaying region and
 *  h_r is a decay factor (akin to scale height)
 *
 */

#ifndef CMODELDISK_C_H_
#define CMODELDISK_C_H_

#include "CModelDisk.h"

class CModelDisk_C: public CModelDisk
{
public:
	CModelDisk_C();
	virtual ~CModelDisk_C();

	double GetRadius(double half_height, double h, double dh, double rim_radius);
};

#endif /* CMODELDISK_B_H_ */
