/*
 * CDataInfo.h
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
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
