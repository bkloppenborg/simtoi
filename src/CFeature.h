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
public:
	CFeature();
	virtual ~CFeature();

	virtual void apply(CModel * model) = 0;
};

#endif /* CFEATURE_H_ */
