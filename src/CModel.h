/*
 * CModel.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A Virtual base class for model objects.
 */

#ifndef CMODEL_H_
#define CMODEL_H_

class CPosition;
class GLFBO;

class CModel
{
protected:
	// Datamembers
	int n_model_parameters
	bool is_analytic;

	double position[3];
	double rotation[3];

public:
	CModel();
	virtual ~CModel();

	void Render();
	void AppendFeature();
	void DeleteFeature();

	int GetTotalParameters();
	void SetParameters(float * params);

	void


};

#endif /* CMODEL_H_ */
