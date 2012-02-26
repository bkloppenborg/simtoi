/*
 * CModelDisk_A.cpp
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 */

#include "CModelDisk_C.h"

CModelDisk_C::CModelDisk_C()
	:CModelDisk(1)
{
	mName = "Disk C";
	mType = CModelList::DISK_C;

	mParamNames.push_back("Beta");
	SetParam(mBaseParams + mDiskParams + 1, 0.05);
	SetFree(mBaseParams + mDiskParams + 1, true);
	SetMax(mBaseParams + mDiskParams + 1, 2.0);
	SetMin(mBaseParams + mDiskParams + 1, 0.01);
}

CModelDisk_C::~CModelDisk_C()
{
	// TODO Auto-generated destructor stub
}

/// Returns the radius for the specified height following exponential decay
double CModelDisk_C::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	const double beta = mParams[mBaseParams + mDiskParams + 1];
	h = fabs(h);

	if(h < dh)
		h = dh;

	return rim_radius * pow(half_height/h, beta);
}
