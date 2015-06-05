/*
 * CFeature.h
 *
 *  Created on: Apr 23, 2014
 *      Author: bkloppenborg
 */

#ifndef CFEATURE_H_
#define CFEATURE_H_

#include <memory>
#include <string>
using namespace std;

#include "CParameterMap.h"

class CModel;

class CFeature: public CParameterMap
{
protected:
	string mName;			///< Human-friendly name for the minimizer.
	string mID;				///< Unique ID for the minimizer.

public:
	CFeature();
	virtual ~CFeature();

	virtual void apply(CModel * model) = 0;

	string ID() { return mID; };
	string name() { return mName; }
};

#endif /* CFEATURE_H_ */
