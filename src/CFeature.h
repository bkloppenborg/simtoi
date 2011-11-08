/*
 * CFeature.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CFEATURE_H_
#define CFEATURE_H_

#include "enumerations.h"

class CFeature
{
protected:
	eFeatureTypes type;
	int n_params_total;
	int n_params_free;


public:
	CFeature();
	virtual ~CFeature();


};

#endif /* CFEATURE_H_ */
