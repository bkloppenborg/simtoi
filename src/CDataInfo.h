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

#ifndef SRC_CDATAINFO_H_
#define SRC_CDATAINFO_H_

#include <string>
#include <cassert>

using namespace std;

/// Lightweight container for data metadata
class CDataInfo {

protected:
	string mFilename;
	string mQuantityDescription;
	double mJDMin;
	double mJDMax;
	double mJDMean;
	double mWavelengthMean;
	double mWavelengthMin;
	double mWavelengthMax;

public:
	CDataInfo();
	virtual ~CDataInfo();

	string filename() const { return mFilename; };
	string description() const { return mQuantityDescription; };
	double JDMin() const { return mJDMin; };
	double JDMax() const { return mJDMax; };
	double JDMean() const { return mJDMean; };
	double wavelengthMin() const { return mWavelengthMin; };
	double wavelengthMax() const { return mWavelengthMax; };
	double wavelengthMean() const { return mWavelengthMean; };

	const CDataInfo getInfo() { return CDataInfo(*this); };

	void setFilename(const string & filename);
	void setDescription(const string & description);
	void setJDMin(double jd);
	void setJDMax(double jd);
	void setJDMean(double jd);
	void setWavelengthMin(double wavelength);
	void setWavelengthMax(double wavelength);
	void setWavelengthMean(double wavelength);
};

#endif /* SRC_CDATAINFO_H_ */
