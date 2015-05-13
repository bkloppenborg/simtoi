/*
 * CDataInfo.cpp
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
 */

#include "CDataInfo.h"

CDataInfo::CDataInfo()
{
	mFilename = "NOT_INITALIZED";
	mQuantityDescription = "NOT_INITALIZED";
	mJDMin = 0;
	mJDMax = 0;
	mJDMean = 0;
	mWavelengthMin = 0;
	mWavelengthMax = 0;
	mWavelengthMean = 0;
}

CDataInfo::~CDataInfo() {
	// TODO Auto-generated destructor stub
}

void CDataInfo::setFilename(const string & filename)
{
	assert(filename.size() > 0);
	mFilename = filename;
}

void CDataInfo::setDescription(const string & description)
{
	assert(description.size() > 0);
	mQuantityDescription = description;
}

void CDataInfo::setJDMin(double jd)
{
	assert(jd > 0);
	mJDMin = jd;
}

void CDataInfo::setJDMax(double jd)
{
	assert(jd > 0);
	mJDMax = jd;
}

void CDataInfo::setJDMean(double jd)
{
	assert(jd > 0);
	mJDMean = jd;
}

void CDataInfo::setWavelengthMin(double wavelength)
{
	assert(wavelength > 0);
	mWavelengthMin = wavelength;
}

void CDataInfo::setWavelengthMax(double wavelength)
{
	assert(wavelength > 0);
	mWavelengthMax = wavelength;
}

void CDataInfo::setWavelengthMean(double wavelength)
{
	assert(wavelength > 0);
	mWavelengthMean = wavelength;
}
