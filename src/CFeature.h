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
	string id;
	string name;

public:
	CFeature();
	virtual ~CFeature();

	virtual void apply(CModel * model) = 0;

	virtual string getID() const { return id; };
	virtual string getName() const { return name; }
};

#endif /* CFEATURE_H_ */
