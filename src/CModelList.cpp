/*
 * CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CModel.h"
#include "CModelList.h"
#include "CModelSphere.h"


using namespace std;

CModelList::CModelList()
{
	// TODO Auto-generated constructor stub

}

CModelList::~CModelList()
{
	// Deallocate the models
	for(int i = mModels.size() - 1; i > -1; i--)
		delete mModels[i];
}

void CModelList::Append(CModel * model)
{
	mModels.push_back(model);
}

/// Returns the total number of free parameters for all models
int CModelList::GetNFreeParameters()
{
	// TODO: We could probably cache this and/or update it as models are added.
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	n += (*it)->GetTotalFreeParameters();
    }

    return n;
}

/// Returns a pair of model names, and their enumerated types
vector< pair<eModels, string> > CModelList::GetList_AllModels(void)
{
	vector< pair<eModels, string> > tmp;
	tmp.push_back(pair<eModels, string> (MDL_SPHERE, "Sphere"));

	return tmp;
}

/// Creates a new model, appends it to the model list and returns a pointer to it.
CModel * CModelList::AddNewModel(eModels model_id)
{
	CModel * tmp;
	switch(model_id)
	{
	case MDL_SPHERE:
		tmp = new CModelSphere();
		break;

	default:
		tmp = NULL;
		break;
	}

	this->Append(tmp);
	return mModels.back();
}

/// This function gets the parameters for models after they have been set/scaled.
/// also useful for determining initial, user-specified values in configuration files.
void CModelList::GetParameters(float * params, int n_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->GetParams(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}

// Render the image to the specified OpenGL framebuffer object.
void CModelList::Render(GLuint fbo, int width, int height)
{
	// First clear the buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->Render(fbo, width, height);
    }

    // Bind back to the default framebuffer and let OpenGL finish:
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glFinish();
}

// This function sets the parameters for models prior to a GetData call.
void CModelList::SetParameters(float * params, int n_params)
{
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = mModels.begin(); it != mModels.end(); ++it)
    {
    	(*it)->SetAllParameters(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}
