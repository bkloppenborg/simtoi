/*
 * CPhotometry.h
 *
 *  Created on: May 12, 2015
 *      Author: bkloppenborg
 */

#ifndef SRC_DATA_CPHOTOMETRY_H_
#define SRC_DATA_CPHOTOMETRY_H_

#include "CData.h"

class CPhotometryHandler;

class CPhotometry : public CData {
	friend class CPhotometryHandler;	// provides access to protected variables

protected:
	vector<double> mJDs;
	vector<double> mValues;
	vector<double> mUncertainties;
	vector<double> mWavelengths;

public:
	CPhotometry();
	virtual ~CPhotometry();

	virtual CDataInfo open(const string & filename);
	virtual void saveAsText(const string & directory);
	virtual void save(const string & filename);

	virtual unsigned int size();

	double lookupWavelength(const string & wavelength_or_band);
};

#endif /* SRC_DATA_CPHOTOMETRY_H_ */
