/*
 * CFeatureList.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CFEATURELIST_H_
#define CFEATURELIST_H_

#include <GL/gl.h>
#include <vector>

#include "CFeature.h"

using namespace std;

// A container for a list of models.
class CFeatureList
{
	vector<CFeature*> features;

private:
	bool SortModelPredicate(CFeature * A, CFeature * B);

public:
	CFeatureList();
	~CFeatureList();

	void Append(CFeature * feature);

	// TODO: implement this function
	//void GetParams(double * params, int n_params);
	int GetNFreeParameters();

	void SetParams(float * params, int n_params);

};

#endif /* CFEATURELIST_H_ */
