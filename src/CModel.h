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
#include <string>

#include "CPosition.h"
#include "CGLShaderList.h"
#include "CGLShaderWrapper.h"
#include "CModelList.h"

using namespace std;

class CPosition;
//class CFeature;
//class CFeatureList;
class CGLShaderWrapper;

class CModel
{
protected:
	// Datamembers
	int n_free_parameters;
	bool is_analytic;

	double rotation[3];

	CPosition * position;
//	CFeatureList * features;

	double * scale;
	double * scale_min;

	string mName;
	eModels mType;

private:
	CGLShaderWrapper * mShader;
	bool mShaderLoaded;

protected:
	void Rotate();
	void Translate();

	// Set the parameters in this model, scaling from a uniform hypercube to physical units as necessary.
	virtual void SetModelParameters(float * params, int n_params) = 0;
	virtual void GetModelParameters(float * params, int n_params) = 0;

public:
	CModel();
	~CModel();

	//void AppendFeature(CFeature * feature);
	//void DeleteFeature();

	int GetTotalFreeParameters();
	eModels GetType(void) { return mType; };
	void GetParameters(float * params, int n_params);
	virtual int GetNModelFreeParameters() = 0;
	int GetNPositionFreeParameters();
	int GetNFeatureFreeParameters();

	virtual void Render(GLuint framebuffer_object, int width, int height) = 0;

	void SetParameters(float * params, int n_params);
	void SetPositionType(ePositionTypes type);
	void SetShader(CGLShaderWrapper * shader);
	bool ShaderLoaded(void) { return mShaderLoaded; };

protected:
	void UseShader();


};

#endif /* CMODEL_H_ */
