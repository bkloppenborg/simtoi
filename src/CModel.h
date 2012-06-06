/*
 * CModel.h
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  A Virtual base class for model objects.
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
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
#include "CCL_GLThread.h"
//#include "enumerations.h"
#include "CModelList.h"

#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cassert>

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795028841968
#endif // M_PI
#endif // PI

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

	CModelList::ModelTypes mType;

	CPosition * mPosition;

//	CFeatureList * features;

	CGLShaderWrapper * mShader;
	bool mShaderLoaded;
	double mScale;

protected:
	void Color();
	void Rotate();
	void Translate();

public:
	static void CircleTable( double * sint, double * cost, const int n );

	// Set the parameters in this model, scaling from a uniform hypercube to physical units as necessary.
	void GetFreeParameters(double * params, int n_params, bool scale_params);
	vector<string> GetFreeParameterNames();
	double GetFreePriorProd();
	vector< pair<double, double> > GetFreeParamMinMaxes();
	void SetFreeParameters(double * params, int n_params, bool scale_params);
	void GetAllParameters(double * params, int n_params);

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
	CModelList::ModelTypes GetType(void) { return mType; };

	virtual void Render(GLuint framebuffer_object, int width, int height) = 0;
public:
	void Restore(Json::Value input, CGLShaderList * shader_list);

public:
	Json::Value Serialize();
	void SetAnglesFromPosition();
	void SetPositionType(CPosition::PositionTypes type);
	void SetShader(CGLShaderWrapper * shader);
	void SetTime(double time);
protected:
	void SetupMatrix();
public:
	bool ShaderLoaded(void) { return mShaderLoaded; };

protected:
	void UseShader();


};

#endif /* CMODEL_H_ */
