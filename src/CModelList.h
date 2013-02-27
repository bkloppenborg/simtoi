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

#include "CPosition.h"

class CModel;
typedef shared_ptr<CModel> CModelPtr;

/// \brief A container class for a list of models.
class CModelList
{
protected:
	vector<CModelPtr> mModels;

protected:
	double mTime;	///< The current time for the models in this list.

public:
	CModelList();
	virtual ~CModelList();

	void AddModel(CModelPtr model);

	int GetNFreeParameters();
	void GetAllParameters(double * params, int n_params);
	vector< pair<double, double> > GetFreeParamMinMaxes();
	void GetFreeParameters(double * params, int n_params, bool scale_params);
	void GetFreeParametersScaled(double * params, int n_params);
	void GetFreeParameterSteps(double * steps, unsigned int size);
	vector<string> GetFreeParamNames();
	CModelPtr GetModel(int i) { return mModels.at(i); };
	double GetTime() { return mTime; };

	static vector<string> GetTypes(void);

	void Render(GLuint fbo, int width, int height);
	void Restore(Json::Value input);

	Json::Value Serialize();
	void SetFreeParameters(double * params, unsigned int n_params, bool scale_params);
	void SetTime(double t);
	void SetTimestep(double dt);
	unsigned int size() { return mModels.size(); };

	static bool SortByZ(const CModelPtr & A, const CModelPtr & B);

};

#endif /* CMODELLIST_H_ */
