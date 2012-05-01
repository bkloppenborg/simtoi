/*
 * CModelDisk_A.h
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 *
 *  Disk Model A:
 *  This model modifies the edges of the simple CModelDisk to extend radially outward
 *  with an exponentially decaying height:
 *
 *  	h(r) = h0 * exp( -(r-r0)/h_r )
 *
 *  where h0 is the initial height, r0 is the inner radius of the decaying region and
 *  h_r is a decay factor (akin to scale height)
 *
 */

#ifndef CMODELDISK_A_H_
#define CMODELDISK_A_H_

#include "CModelDisk.h"

class CModelDisk_A: public CModelDisk
{
public:
	CModelDisk_A();
	virtual ~CModelDisk_A();

	double GetRadius(double half_height, double h, double dh, double rim_radius);
};

#endif /* CMODELDISK_A_H_ */
