/*
 * CDisk_Andrews2009.h
 *
 *  Created on: Nov, 5, 2013
 *      Author: bkloppenborg
 *
 *  A class implementing a disk model consisting of a power-law internal density
 *  profile and an exponential taper at larger radii (c.f. Andrews et al. 2009)
 */

#ifndef CDISK_ANDREWS2009_H_
#define CDISK_ANDREWS2009_H_

#include "CDensityDisk.h"

class CDisk_Andrews2009: public CDensityDisk
{

public:
	CDisk_Andrews2009();
	virtual ~CDisk_Andrews2009();

	static shared_ptr<CModel> Create();

	double Density(double radius, double height);

	virtual string GetID() { return "disk_andrews2009"; };

	double Opacity(double radius, double height, double wavelength);
};

#endif /* CDISK_ANDREWS2009_H_ */
