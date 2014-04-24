/*
 * CFeature.h
 *
 *  Created on: Apr 23, 2014
 *      Author: bkloppenborg
 */

#ifndef CFEATURE_H_
#define CFEATURE_H_

#include <memory>

#include "CParameters.h"

class CModel;

class CFeature: public CParameters
{
public:
	CFeature(int n_params);
	virtual ~CFeature();

	virtual string GetID() { return "invalid_feature"; };

	virtual void apply(CModel * model) = 0;
};

#endif /* CFEATURE_H_ */
