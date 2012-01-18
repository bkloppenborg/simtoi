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

	// Varaibles for parameter values
	int mNParams;
	float * mParams;
	int mNFreeParams;
	bool * mFreeParams;
	float * mScales;
	float * mScale_mins;
	vector<string> mParamNames;

//	CFeatureList * features;

	string mName;
	eModels mType;

private:
	CGLShaderWrapper * mShader;
	bool mShaderLoaded;

protected:
	void Rotate();
	void Translate();

	// Set the parameters in this model, scaling from a uniform hypercube to physical units as necessary.
	void SetAllParameters(float * params, int n_params);
	void GetAllParameters(float * params, int n_params);

public:
	CModel(int n_params);
	~CModel();

	//void AppendFeature(CFeature * feature);
	//void DeleteFeature();

	int GetTotalFreeParameters();
	eModels GetType(void) { return mType; };
protected:
	void GetParams(float * params, int n_params);
public:
	int GetNModelFreeParameters();
	int GetNPositionFreeParameters();
	int GetNFeatureFreeParameters();

	virtual void Render(GLuint framebuffer_object, int width, int height) = 0;

protected:
	void SetParams(float * in_params, int n_params);
public:
	void SetPositionType(ePositionTypes type);
	void SetShader(CGLShaderWrapper * shader);
	bool ShaderLoaded(void) { return mShaderLoaded; };

protected:
	void UseShader();


};

#endif /* CMODEL_H_ */
