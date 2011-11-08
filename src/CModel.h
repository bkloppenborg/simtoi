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

// Headers for OpenGL functions
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "CPosition.h"
#include "enumerations.h"

class CPosition;
class CFeature;
class CFeatureList;


class CModel
{
protected:
	// Datamembers
	int n_free_parameters;
	bool is_analytic;

	double rotation[3];

	CPosition * position;
	CFeatureList * features;

protected:
	void Rotate();
	void Translate();
	virtual void SetModelParameters(double * params, int n_params) = 0;

public:
	CModel();
	~CModel();

	void AppendFeature(CFeature * feature);
	//void DeleteFeature();

	int GetTotalFreeParameters();
	virtual int GetNModelFreeParameters() = 0;
	int GetNPositionFreeParameters();
	int GetNFeatureFreeParameters();

	virtual void Render(GLuint framebuffer_object, int width, int height) = 0;

	void SetParameters(double * params, int n_params);
	void SetPositionType(ePositionTypes type);


};

#endif /* CMODEL_H_ */
