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

#include "CParameters.h"
#include "CPosition.h"
#include "CGLShaderList.h"
#include "CGLShaderWrapper.h"
#include "CGLThread.h"
#include "enumerations.h"

using namespace std;

class CPosition;
//class CFeature;
//class CFeatureList;
class CGLShaderWrapper;

class CModel : public CParameters
{
protected:
	// Datamembers
//	bool is_analytic;
	int mBaseParams;

	eModels mType;

	CPosition * mPosition;

//	CFeatureList * features;

	CGLShaderWrapper * mShader;
	bool mShaderLoaded;

protected:
	void Color();
	void Rotate();
	void Translate();

public:
	// Set the parameters in this model, scaling from a uniform hypercube to physical units as necessary.
	void SetAllParameters(float * params, int n_params);
	void GetAllParameters(float * params, int n_params);

public:
	CModel(int n_params);
	~CModel();

	//void AppendFeature(CFeature * feature);
	//void DeleteFeature();

	int GetNFeatureFreeParameters() { return 0; };
	int GetNModelFreeParameters() { return mNFreeParams; };
	int GetNPositionFreeParameters() { return mPosition->GetNFreeParams(); };
	int GetNShaderFreeParameters() { return mShader->GetNFreeParams(); };
	CPosition * GetPosition(void) { return mPosition; };
	CGLShaderWrapper * GetShader(void) { return mShader; };
	int GetTotalFreeParameters();
	eModels GetType(void) { return mType; };

	virtual void Render(GLuint framebuffer_object, int width, int height) = 0;

public:
	void SetPositionType(ePositionTypes type);
	void SetShader(CGLShaderWrapper * shader);
	void SetTime(double time);
	bool ShaderLoaded(void) { return mShaderLoaded; };

protected:
	void UseShader();


};

#endif /* CMODEL_H_ */
