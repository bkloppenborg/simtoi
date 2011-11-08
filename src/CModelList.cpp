/*
 * CCModelList.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */


#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

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

// Render the image to the specified OpenGL framebuffer object.
void CModelList::Render(GLuint framebuffer_object, int width, int height)
{
	// First clear the buffer.
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);
    glClearColor (0.0f, 0.0f, 0.0f, 0.0f); // Set the clear color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the depth and color buffers

    CModel * model;
    // Now call render on all of the models:
    for(vector<CModel*>::iterator it = models.begin(); it != models.end(); ++it)
    {
    	(*it)->Render(framebuffer_object, width, height);
    }

    // Bind back to the default buffer (just in case a model forgot to do so),
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blit the application-defined render buffer to the on-screen render buffer.
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_object);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_BACK);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    // Tell OpenGL to finish operations then swap buffers to display the rendered model.
    glFinish();
    glutSwapBuffers();
}

// This function sets the parameters for models prior to a GetData call.
void CModelList::SetParameters(double * params, int n_params)
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

void CModelList::GetData()
{
}
