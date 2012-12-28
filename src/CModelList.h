/*
 * CModelList.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  A Class for storing and loading models.
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

#ifndef CMODELLIST_H_
#define CMODELLIST_H_

#include <string>
#include <GL/gl.h>
#include <GL/glu.h>
#include <memory>
#include <vector>
using namespace std;

#include "enumerations.h"
#include "CPosition.h"

class CModel;
class CGLShaderWrapper;
class CGLShaderList;

typedef shared_ptr<CModel> CModelPtr;
typedef shared_ptr<CGLShaderWrapper> CGLShaderWrapperPtr;

// A container for a list of models.
class CModelList
{
protected:
	vector<CModelPtr> mModels;

public:
	/// Enumerated Model Names.
	/// Note, when adding a new model, list it in this enum and add it to functions:
	///       GetTypes() and in GetNewModel().  We want to change this, see issue #50.
	enum ModelTypes
	{
		NONE,
		SPHERE,
		DISK,
		DISK_A,
		DISK_B,
		DISK_C,
		DISK_CONCENTRIC_RINGS,
		LAST_VALUE // must be the last value in this list.
	};

protected:
	double mTime;
	double mTimestep;

public:
	CModelList();
	virtual ~CModelList();

	CModelPtr AddNewModel(ModelTypes model_id);

	int GetNFreeParameters();
	void GetAllParameters(double * params, int n_params);
	vector< pair<double, double> > GetFreeParamMinMaxes();
	void GetFreeParameters(double * params, int n_params, bool scale_params);
	void GetFreeParametersScaled(double * params, int n_params);
	double GetFreeParameterPriorProduct();
	vector<string> GetFreeParamNames();
	CModelPtr GetModel(int i) { return mModels.at(i); };
	double GetTime() { return mTime; };

	static vector< pair<ModelTypes, string> > GetTypes(void);

	void IncrementTime();

	void Render(GLuint fbo, int width, int height);
	void Restore(Json::Value input, CGLShaderList * shader_list);

	Json::Value Serialize();
	void SetFreeParameters(double * params, unsigned int n_params, bool scale_params);
	void SetPositionType(unsigned int model_id, CPosition::PositionTypes pos_type);
	void SetShader(unsigned int model_id, CGLShaderWrapperPtr shader);
	void SetTime(double t);
	void SetTimestep(double dt);
	unsigned int size() { return mModels.size(); };

};

#endif /* CMODELLIST_H_ */
