/*
 * CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#include "CModelList.h"

using namespace std;

CModelList::CModelList()
{
	// TODO Auto-generated constructor stub

}

CModelList::~CModelList()
{
	// Deallocate the models
	for(int i = models.size() - 1; i > -1; i--)
		delete models[i];
}

void CModelList::Append(CModel * model)
{
	this->models.push_back(model);
}

/// Returns the total number of free parameters for all models
int CModelList::GetNFreeParameters()
{
	// TODO: We could probably cache this and/or update it as models are added.
    CModel * model;
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = models.begin(); it != models.end(); ++it)
    {
    	n += (*it)->GetTotalFreeParameters();
    }

    return n;
}

/// This function gets the parameters for models after they have been set/scaled.
/// also useful for determining initial, user-specified values in configuration files.
void CModelList::GetParameters(float * params, int n_params)
{
    CModel * model;
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = models.begin(); it != models.end(); ++it)
    {
    	(*it)->GetParameters(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}

// Render the image to the specified OpenGL framebuffer object.
void CModelList::Render(COpenGL * gl)
{
	GLuint fbo = gl->GetFramebuffer();
	int width = gl->GetWindowWidth();
	int height = gl->GetWindowHeight();

	// First clear the buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers

    CModel * model;
    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = models.begin(); it != models.end(); ++it)
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
    CModel * model;
    int n = 0;

    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = models.begin(); it != models.end(); ++it)
    {
    	(*it)->SetParameters(params + n, n_params - n);
    	n += (*it)->GetTotalFreeParameters();
    }
}
