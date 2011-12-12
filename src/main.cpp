/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
 *
 *  File implementing the main routines for SIMTOI.
 *
 *  Note: The implementation of SIMTOI's drawing routines may seem a little odd to an OpenGL programmer.
 *  Here GLUT is used only to display the current model and provide minimial user interaction.  All rendering
 *  is controlled by the minimization algorithm and it calls glutPostRedisplay.
 *
 */

#ifndef MAIN_CPP_
#define MAIN_CPP_

#include <string>
#include <iostream>
#include <cstdio>

// OpenGL, liboi headers
#include "COpenGL.h"
#include "CLibOI.h"

#include "main.h"
//#include "CMinimizer.h"
#include "CModelList.h"
#include "CModelSphere.h"
#include <boost/thread/thread.hpp>

using namespace std;

// Globals
int window_width = 128;
int window_height = 128;
CModelList * models;

// OpenGL/OpenCL Globals:
COpenGL * gl;
CLibOI * ocl;
//CMinimizer * minimizer;

/// Clean up OpenGL stuff.
void cleanup()
{
	// Clean up memory
    if(ocl) delete ocl;
    if(models) delete models;
    if(gl) delete gl;
}

/// Display function.
void display(void)
{
	// Simply blit the offscreen to the screen.
	gl->BlitToScreen();
}

// Prints out help describing the options on the command line
void print_help()
{
	string usage = "";

	cout << usage << "\n";
}

void keyboard(unsigned char key, int x, int y)
{
    if(key == 'q' || key == 'Q' || key == '\27')
    {
    	// Call the freeglut leave function that way we may safely clean up resources.
    	cleanup();
    	glutLeaveMainLoop();
    }

    // Display on the onscreen buffer.
    if(key == 'd')
    {
        glutPostRedisplay();
    }

    // Render to the offscreen buffer
    if(key == 'r')
    {
    	double junk[5] = {2, 0, 0, 0.9, 0};
    	models->SetParameters(junk, 5);
    	models->Render(gl);
    }

    if(key == 'R')
    {
    	double junk[5] = {3, 0, 0, 0.9, 0};
    	models->SetParameters(junk, 5);
    	models->Render(gl);
    }

    if(key == 'c')
    {
    	float tmp = ocl->ImageToChi2(0);
    	printf("Data 0 chi2: %f\n", tmp);
    }
}

void reshape (int w, int h)
{
    // Do nothing, we don't want to alter the scale.
}


// The main routine.
int main(int argc, char *argv[])
{
	// Because the program is OpenGL + OpenCL we must init this way:
	// 1. Load modeling info (window size, scale)
	// 2. init OpenGL
	// 3. Load the model parameters, init the models.
	// 4. init OpenCL
	// 4a  Load OIFITS data
	// 4b  Init OpenCL Memory and routines
	// 5. init the minimizer, spawn off as a thread.
	// 6. Start the OpenGL event loop.

	window_width = 128;
	window_height = 128;
	double scale = 0.05;

	// Create a GL object, init and register callbacks:
	gl = new COpenGL(window_width, window_height, scale, "/home/bkloppenborg/workspace/simtoi/src/shaders");
    gl->init(argc, argv);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Now create and init models for SIMTOI
	models = new CModelList();
    // Load the models, shaders, and features
    // TODO: Read this in from a text file.
	CShader * shader;
	shader = gl->GetShader(LD_Hesteroffer1997);
	CModel * tmp = new CModelSphere();
	tmp->SetPositionType(XY);
	tmp->SetShader(shader);
	models->Append(tmp);

	// Initialize OpenCL
	ocl = new CLibOI();
	ocl->SetKernelSourcePath("/home/bkloppenborg/workspace/simtoi/lib/liboi/src/kernels");
	ocl->Init(CL_DEVICE_TYPE_GPU, window_width, window_height, 1, scale);
	ocl->RegisterImage_GLTB(gl->GetFramebufferTexture());

	// Load OIFITS data into memory
	ocl->LoadData("/home/bkloppenborg/workspace/simtoi/bin/epsaur.oifits");

	// Now init memory and routines
	ocl->InitMemory();
	ocl->InitRoutines();

	// Now start the minimizer

    // Start the main loop:
    glutMainLoop();
    cleanup();

	return 0;
}


#endif /* MAIN_CPP_ */
