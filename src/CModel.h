/*
 * \file CModel.h
 *
 *  Created on: Nov 7, 2011
 *  \author bkloppenborg
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

#include "OpenGL.h" // OpenGL includes, plus several workarounds for various OSes

#include "CParameter.h"
#include "CPosition.h"
#include "CShader.h"
#include "CWorkerThread.h"
#include "CModelList.h"
#include "CParameterMap.h"

#include <cstdlib>
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cassert>
#include <memory>

// OpenGL Math Library code.
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;

using namespace std;

#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.1415926535897932384626433832795028841968
#endif // M_PI
#endif // PI

class CPosition;
typedef shared_ptr<CPosition> CPositionPtr;

class CShader;
typedef shared_ptr<CShader> CShaderPtr;

class CFeature;
typedef shared_ptr<CFeature> CFeaturePtr;

/// \brief A base class for all models in SIMTOI
///
/// This class serves as a basis from which all models in SIMTOI are derived.
/// It implements methods for saving and restoring (`Serialize()` and `Restore()`)
/// models from SIMTOI save files and for basic OpenGL operations such as rotation
/// and translation.
///
/// At a minimum, all models should override the `Render()` and `GetID()` functions.
/// See `CModelSphere` for a simple example.
///
/// This class is derived from `CParameters` which is used to store the
/// values of model parameters.
class CModel: public CParameterMap
{

protected:
	string id;
	string name;

	CPositionPtr mPosition;	///< A shared pointer to the position object.

	CShaderPtr mShader;	///< A shared pointer to the shader object.
	double mScale;

	GLuint mFluxTextureID; // texture id
	vector<double> mPixelTemperatures;
	vector<vec4> mFluxTexture;

	vector<CFeaturePtr> mFeatures;

protected:
	glm::mat4 Rotate();
	glm::mat4 Translate();

public:
	static void CircleTable( double * sint, double * cost, const int n );

	virtual void FindPixels(double s0, double s1, double s2,
			double ds0, double ds1, double ds2,
			vector<unsigned int> &pixels_ids);

	void GetAllParameters(double * params, int n_params);
	// Set the parameters in this model, scaling from a uniform hypercube to physical units as necessary.
	void GetFreeParameters(double * params, int n_params, bool scale_params);
	vector<string> GetFreeParameterNames();
	vector< pair<double, double> > GetFreeParamMinMaxes();
	void GetFreeParameterSteps(double * steps, unsigned int size);

	void SetFreeParameters(double * params, int n_params, bool scale_params);


public:
	CModel();
	virtual ~CModel();

	void AddFeature(string feature_id);

	virtual string GetID() const { return id; };
	virtual string GetName() const { return name; };
	int GetNModelFreeParameters();
	int GetNPositionFreeParameters();
	int GetNShaderFreeParameters();
	int GetNFeatureFreeParameters();
	vector<double> & GetPixelTemperatures();
	CPositionPtr GetPosition(void);
	CShaderPtr GetShader(void);

	const vector<CFeaturePtr> & GetFeatures() const;;

	int GetTotalFreeParameters();

protected:
	virtual void InitTexture();
	virtual void InitShaderVariables();

public:
	virtual void Render(GLuint framebuffer_object, const glm::mat4 & view) = 0;
public:
	void Restore(Json::Value input);

public:
	Json::Value Serialize();
	void SetPositionModel(string position_id);
	void SetPositionModel(CPositionPtr position);

	virtual void SetShader(string shader_id);
	virtual void SetShader(CShaderPtr shader);
	virtual void SetTime(double time);
protected:
	void SetupMatrix();

public:
	static void TemperatureToFlux(vector<double> temperatures, vector<float> & fluxes,
			double wavelength, double max_temperature);
	static void TemperatureToFlux(vector<double> temperatures, vector<vec4> & pixels,
			double wavelength, double max_temperature);

};

#endif /* CMODEL_H_ */
