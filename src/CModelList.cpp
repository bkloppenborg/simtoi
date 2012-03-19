/*
 * CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CModelList.h"

#include <sstream>

#include "CCL_GLThread.h"
#include "CPosition.h"
#include "CGLShaderList.h"

// Models
#include "CModel.h"
#include "CModelSphere.h"
#include "CModelDisk.h"
#include "CModelDisk_A.h"
#include "CModelDisk_B.h"
#include "CModelDisk_C.h"

using namespace std;

CModelList::CModelList()
{
	mTime = 0;
	mTimestep = 0;
}

CModelList::~CModelList()
{

}

/// Creates a new model, appends it to the model list and returns a pointer to it.
CModel * CModelList::AddNewModel(ModelTypes model_id)
{
	CModel * tmp;
	switch(model_id)
	{
	case DISK:
		tmp = new CModelDisk();
		break;

	case DISK_A:
		tmp = new CModelDisk_A();
		break;

	case DISK_B:
		tmp = new CModelDisk_B();
		break;

	case DISK_C:
		tmp = new CModelDisk_C();
		break;

	case SPHERE:
	default:
		tmp = new CModelSphere();
		break;
	}

	this->Append(tmp);
	return mList.back();
}

/// Returns the total number of free parameters in the models
int CModelList::GetNFreeParameters()
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	n +=(*it)->GetTotalFreeParameters();
    }

    return n;
}

void CModelList::GetAllParameters(double * params, int n_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	(*it)->GetAllParameters(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}

vector< pair<double, double> > CModelList::GetFreeParamMinMaxes()
{
    vector< pair<double, double> > tmp1;
    vector< pair<double, double> > tmp2;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	tmp2 = (*it)->GetFreeParamMinMaxes();
    	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
    }

    return tmp1;
}

/// Gets the values for all of the free parameters.
void CModelList::GetFreeParameters(double * params, int n_params, bool scale_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
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
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	tmp2 = (*it)->GetFreeParameterNames();
    	tmp1.insert( tmp1.end(), tmp2.begin(), tmp2.end() );
    }

    return tmp1;
}

/// Returns a pair of model names, and their enumerated types
vector< pair<CModelList::ModelTypes, string> > CModelList::GetTypes(void)
{
	vector< pair<ModelTypes, string> > tmp;
	tmp.push_back(pair<ModelTypes, string> (CModelList::SPHERE, "Sphere"));
	tmp.push_back(pair<ModelTypes, string> (CModelList::DISK, "Disk - Cylinder"));
	tmp.push_back(pair<ModelTypes, string> (CModelList::DISK_A, "Disk - A"));
	tmp.push_back(pair<ModelTypes, string> (CModelList::DISK_B, "Disk - B"));
	tmp.push_back(pair<ModelTypes, string> (CModelList::DISK_C, "Disk - C"));

	return tmp;
}

/// Returns the product of priors from all models
double CModelList::GetFreeParameterPriorProduct()
{
	double tmp = 1;

    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    	tmp *= (*it)->GetFreePriorProd();

    return tmp;
}

/// Increments the time by the set timestep value.
void CModelList::IncrementTime()
{
	SetTime(mTime + mTimestep);
}

// Render the image to the specified OpenGL framebuffer object.
void CModelList::Render(GLuint fbo, int width, int height)
{
	// First clear the buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	(*it)->Render(fbo, width, height);
    }

    // Bind back to the default framebuffer and let OpenGL finish:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFlush();
    glFinish();
}

/// Restores the saved models
void CModelList::Restore(Json::Value input, CGLShaderList * shader_list)
{
	// Clear the list if there are already models in it.
	if(mList.size() > 0)
		this->Clear();

	CModelList::ModelTypes type = CModelList::NONE;
	CModel * model;
	Json::Value tmp;
	int value = 0;

	Json::Value::Members members = input.getMemberNames();
	for(unsigned int i = 0; i < members.size(); i++)
	{
		value = input[members[i]]["base"]["type"].asInt();
		tmp = input[members[i]];
		type = CModelList::ModelTypes( value );
		model = AddNewModel(type);
		model->Restore(tmp, shader_list);
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
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	Json::Value tmp;
    	name.str("");
    	name << "model_" << i;
    	output[name.str()] = (*it)->Serialize();
//    	tmp[name.str()] = (*it)->Serialize();
//    	tmp.setComment("// Next Model", Json::commentBefore);
//    	output.append(tmp);
    	i++;
    }

    return output;
}

/// Sets all of the free parameter values
void CModelList::SetFreeParameters(double * params, int n_params, bool scale_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	(*it)->SetFreeParameters(params + n, n_params - n, scale_params);
    	n += (*it)->GetTotalFreeParameters();
    }
}

void CModelList::SetPositionType(unsigned int model_id, CPosition::PositionTypes pos_type)
{
	if(model_id < mList.size())
		mList[model_id]->SetPositionType(pos_type);
}

void CModelList::SetShader(unsigned int model_id, CGLShaderWrapper * shader)
{
	if(model_id < mList.size())
		mList[model_id]->SetShader(shader);
}

/// Sets the time for all of the models
/// Note, some modes don't care about time
void CModelList::SetTime(double t)
{
	mTime = t;
    for(vector<CModel*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	(*it)->SetTime(mTime);
    }
}

/// Sets the time increment (for use with animation).
void CModelList::SetTimestep(double dt)
{
	mTimestep = dt;
}
