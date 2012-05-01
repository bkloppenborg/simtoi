/*
 * CModelDisk_A.cpp
 *
 *  Created on: Feb 24, 2012
 *      Author: bkloppenborg
 */

#include "CModelDisk_A.h"

CModelDisk_A::CModelDisk_A()
	:CModelDisk(1)
{
	mName = "Disk A";
	mType = CModelList::DISK_A;

	mParamNames.push_back("Decay Factor");
	SetParam(mBaseParams + mDiskParams + 1, 0.05);
	SetFree(mBaseParams + mDiskParams + 1, true);
	SetMax(mBaseParams + mDiskParams + 1, 2.0);
	SetMin(mBaseParams + mDiskParams + 1, 0.01);
}

CModelDisk_A::~CModelDisk_A()
{
	// TODO Auto-generated destructor stub
}

/// Returns the radius for the specified height following exponential decay
double CModelDisk_A::GetRadius(double half_height, double h, double dh, double rim_radius)
{
	const double h_r = mParams[mBaseParams + mDiskParams + 1];
	h = fabs(h);

	if(h < dh)
		h = dh;

	return rim_radius - h_r * log(h/half_height);
}
