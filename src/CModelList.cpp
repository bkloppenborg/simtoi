/*
 * \file CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 * \author: bkloppenborg
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

#include "CModelList.h"

#include <sstream>
#include <algorithm>

#include "CModel.h"
#include "CModelFactory.h"
#include "CCL_GLThread.h"

using namespace std;

/// \brief Default constructor
CModelList::CModelList()
{
	mTime = 0;
}

CModelList::~CModelList()
{

}

/// \brief Adds a new model to the list
void CModelList::AddModel(CModelPtr model)
{
	mModels.push_back(model);
}


/// \brief Returns the total number of free parameters in all models
int CModelList::GetNFreeParameters()
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	n +=(*it)->GetTotalFreeParameters();
    }

    return n;
}

/// \brief Yields the values of all of the parameters in the models
///
/// \param params A pre-allocated array of size `n_params` into which the
/// 	parameter values will be placed.
/// \param n_params The size of params.
void CModelList::GetAllParameters(double * params, int n_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->GetAllParameters(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}

/// \brief Get the free parameter min/maxes
vector< pair<double, double> > CModelList::GetFreeParamMinMaxes()
{
    vector< pair<double, double> > tmp1;
    vector< pair<double, double> > tmp2;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	tmp2 = (*it)->GetFreeParamMinMaxes();
    	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
    }

    return tmp1;
}

/// \brief Gets the values for all of the free parameters.
void CModelList::GetFreeParameters(double * params, int n_params, bool scale_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->GetFreeParameters(params + n, n_params - n, scale_params);
    	n += (*it)->GetTotalFreeParameters();
    }
}

/// Returns a vector of string containing the parameter names.
vector<string> CModelList::GetFreeParamNames()
{
    vector<string> tmp1;
    vector<string> tmp2;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	tmp2 = (*it)->GetFreeParameterNames();
    	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
    }

    return tmp1;
}

/// Returns a pair of model names, and their enumerated types
vector<string> CModelList::GetTypes(void)
{
	auto factory = CModelFactory::Instance();
	return factory.GetModelList();
}

// Render the image to the specified OpenGL framebuffer object.
void CModelList::Render(GLuint fbo, int width, int height)
{
	// We render the models in order by depth (i.e. z-direction).
	// To do this, we do a shallow copy of the model vector, then sort by z.
	vector<CModelPtr> models = mModels;
	sort(models.begin(), models.end(), SortByZ);

	// First clear the buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers

    // Now call render on all of the models:
    for(auto model : models)
    {
    	model->Render(fbo, width, height);
    }

    // Bind back to the default framebuffer and let OpenGL finish:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

/// Restores the saved models
void CModelList::Restore(Json::Value input)
{
	auto factory = CModelFactory::Instance();
	CModelPtr model;

	// Clear the list:
	mModels.clear();

	string model_id = "";
	Json::Value tmp;

	// Iterate through the members in the file, get their model_id, and restore the objects.
	Json::Value::Members members = input.getMemberNames();
	for(unsigned int i = 0; i < members.size(); i++)
	{
		// Look up the type of model and create an object of that type:
		model_id = input[members[i]]["base_id"].asString();
		model = factory.CreateModel(model_id);

		// Now have the model restore the rest of itself, then push it onto the list.
		model->Restore(input[members[i]]);
		AddModel(model);
	}
}

/// Serializes the model list:
Json::Value CModelList::Serialize()
{
	Json::Value output;
	output.setComment("// Model save file from SIMTOI in JSON format.", Json::commentBefore);
	stringstream name;

    // Now call render on all of the models:
	int i = 0;
    for(auto model: mModels)
    {
    	Json::Value tmp;
    	name.str("");
    	name << "model_" << i;
    	output[name.str()] = model->Serialize();
    	i++;
    }

    return output;
}

/// Sets all of the free parameter values
void CModelList::SetFreeParameters(double * params, unsigned int n_params, bool scale_params)
{
	unsigned int n = 0;

    // Now call render on all of the models:
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->SetFreeParameters(params + n, n_params - n, scale_params);
    	n += (*it)->GetTotalFreeParameters();
    }
}

/// Sets the time for all of the models
/// Note, some modes don't care about time
void CModelList::SetTime(double t)
{
	mTime = t;
    for(vector<CModelPtr>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->SetTime(mTime);
    }
}

bool CModelList::SortByZ(const CModelPtr & A, const CModelPtr & B)
{
	double ax, ay, az;
	double bx, by, bz;

	A->GetPosition()->GetXYZ(ax, ay, az);
	B->GetPosition()->GetXYZ(bx, by, bz);

	if(az > bz)
		return true;

	return false;
}
