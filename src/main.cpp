/*
 * main.cpp
 *
 *  Created on: Nov 7, 2011
 *      Author: bkloppenborg
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
#include "CModelList.h"
#include "CModelSphere.h"

using namespace std;

// Globals
int window_width = 128;
int window_height = 128;
CModelList * models;

// OpenGL/OpenCL Globals:
COpenGL * gl;
CLibOI * ocl;

// Prints out help describing the options on the command line
void print_help()
{
	string usage = "";

	cout << usage << "\n";
}

void keyboard(unsigned char key, int x, int y)
{
    if(key == 'q' || key == 'Q' || key == '\27')
        exit(0);

    if(key == 'a')
    {
    	double junk[5] = {3, 0, 0, 0.9, 0};
    	models->SetParameters(junk, 5);
        glutPostRedisplay();
    }

    if(key == 'f')
    {
		float flux = ocl->TotalFlux(true);
		printf("Total flux %f\n", flux);
    }
}

void reshape (int w, int h)
{
    // Do nothing, we don't want to alter the scale.
}

void display(void)
{
	models->Render(gl->GetFramebuffer(), window_width, window_height);
}

// The main routine.
int main(int argc, char *argv[])
{
	window_width = 128;
	window_height = 128;
	double scale = 0.05;

	// Create a GL object, init and register callbacks:
	gl = new COpenGL(window_width, window_height, scale, "/home/bkloppenborg/workspace/simtoi/src/shaders");
    gl->init(argc, argv);
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    //glutIdleFunc(idle);

    // Now create and init models
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
	ocl->SetKernelSoucePath("/home/bkloppenborg/workspace/simtoi/lib/liboi/src/kernels");
	ocl->Init(CL_DEVICE_TYPE_GPU, window_width, window_height, 1);
	ocl->RegisterImage_GLTB(gl->GetFramebufferTexture());

    // Start the main loop:
    glutMainLoop();

	// Clean up memory
    delete ocl;
    delete models;
    delete gl;

	return 0;
}


#endif /* MAIN_CPP_ */
