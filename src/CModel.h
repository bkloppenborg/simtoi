/*
 * CModel.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A Virtual base class for model objects.
 *
 */

#ifndef CMODEL_H_
#define CMODEL_H_

#include "CPosition.h"

class CPosition;
class CFeature;
class GLFBO;

class CModel
{
protected:
	// Datamembers
	int n_model_parameters;
	bool is_analytic;

	double rotation[3];

	CPosition * position;

public:
	CModel();
	~CModel();

	virtual void Render() = 0;
	void AppendFeature(CFeature feature);
	//void DeleteFeature();

	int GetTotalParameters();
	void SetParameters(float * params);


};

#endif /* CMODEL_H_ */
