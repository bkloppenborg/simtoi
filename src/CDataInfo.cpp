 /*
 * This file is part of the SImulation and Modeling Tool for Optical
 * Interferometry (SIMTOI).
 *
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation version 3.
 *
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2015 Brian Kloppenborg
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
