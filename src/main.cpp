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
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/freeglut.h>
#include <boost/thread/thread.hpp>

#include "main.h"
#include "CSIMTOI.h"
#include "CMinimizer.h"


using namespace std;

CSIMTOI * simtoi;
CMinimizer * minimizer;

/// Clean up OpenGL stuff.
void cleanup()
{
	// Clean up memory
    if(simtoi) delete simtoi;
}

/// Display function.
void display(void)
{
	// Simply blit the offscreen to the screen.
	simtoi->BlitToScreen();
}

// Prints out help describing the options on the command line
void print_help()
{
	string usage = "";

	cout << usage << "\n";
}

void keyboard(unsigned char key, int x, int y)
{
	// Call the freeglut leave function that way we may safely clean up resources.
    if(key == 'q' || key == 'Q' || key == '\27')
    	glutLeaveMainLoop();

    // Display on the onscreen buffer.
    if(key == 'd')
    {
        glutPostRedisplay();
    }
}

void reshape (int w, int h)
{
    // Do nothing, we don't want to alter the scale.
}


// The main routine.
int main(int argc, char *argv[])
{
	simtoi = CSIMTOI::GetInstance();
	simtoi->Init(argc, argv);

	// Setup the glut display, reshape, and keyboard functions.
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Init the minimization thread:
    minimizer = CMinimizer::GetMinimizer("MultiNest", simtoi);
    minimizer->Init();
    minimizer->Run();

	glutMainLoop();

	delete simtoi;
	return 0;
}


#endif /* MAIN_CPP_ */
