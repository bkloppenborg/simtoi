/*
 * CDisk_Alpha1973.h
 *
 *  Created on: July, 22, 2015
 *      Author: j robert parks iv
 *
 *  A class implementing a disk model consisting of a power-law internal density
 *  profile and an exponential taper at larger radii.
 *  Based upon the work of Shakura, N. I.; Sunyaev, R. A. 1973
 *  http://adsabs.harvard.edu/abs/1973A%26A....24..337S
 */

#ifndef CDISK_ALPHA1973_H_
#define CDISK_ALPHA1973_H_

#include "CDensityDisk.h"

class CDisk_Alpha1973: public CDensityDisk
{

public:
	CDisk_Alpha1973();
	virtual ~CDisk_Alpha1973();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "disk_alpha1973"; };
};

#endif /* CDISK_ANDREWS2009_H_ */
