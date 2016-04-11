 /*
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
 *
 * Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CMODELLIST_H_
#define CMODELLIST_H_

#include <string>

// OpenGL Headers:
#include "OpenGL.h" // OpenGL includes, plus several workarounds for various OSes

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

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
	double mTime;	///< The current time for the models in this list (JD)
	double mWavelength; ///< The current wavelength of observation (meters)

public:
	CModelList();
	virtual ~CModelList();

	void AddModel(CModelPtr model);

	void clear();

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

	double Render(const glm::mat4 & view);
	void ReplaceModel(unsigned int model_index, CModelPtr model);
	void RemoveModel(unsigned int model_index);
	void Restore(Json::Value input);

	Json::Value Serialize();
	void SetFreeParameters(const double * params, unsigned int n_params, bool scale_params);
	void SetTime(double t);
	void SetTimestep(double dt);
	void SetWavelength(double wavelength);
	unsigned int size() { return mModels.size(); };

	static bool SortByZ(const CModelPtr & A, const CModelPtr & B);

};

#endif /* CMODELLIST_H_ */
