/*
 * CSIMTOI.cpp
 *
 *  Created on: Dec 12, 2011
 *      Author: bkloppenborg
 */

#include "CSIMTOI.h"
#include "COpenGL.h"
#include "CLibOI.h"
// TODO: Perhaps we can figure out how to remove the model headers from here?
#include "CModelList.h"
#include "CModelSphere.h"

CSIMTOI::CSIMTOI(int argc, char *argv[])
{
	// Init the class members with some bogus values:
	mGL = NULL;
	mCL = NULL;
	mModelList = NULL;
	mWindow_width = 1;
	mWindow_height = 1;
	mScale = 1;
	mShaderSourceDir = "";
	mKernelSourceDir = "";

	// initialize
	//ParseCommandLine(argc, argv);
	Init(argc, argv);
}

CSIMTOI::~CSIMTOI()
{
	// Free memory.
	if(mCL) delete mCL;
	if(mGL) delete mGL;
	if(mModelList) delete mModelList;
}

/// Displays the current model to the user using GL_BACK and GL_FRONT buffers.
void CSIMTOI::BlitToScreen()
{
	mGL->BlitToScreen();
}

float CSIMTOI::GetChi2(int data_num, int wl_num)
{
	mCL->CopyImageToBuffer(wl_num);
	return mCL->ImageToChi2(data_num);
}

float CSIMTOI::GetLogLike(int data_num, int wl_num)
{
//	mCL->CopyImageToBuffer(wl_num);
//	mCL->ImageToLogLike(data_num);

	return 0;
}

/// Gets the current model parameters, scaled to physical units.
/// Returns the first 'size' of them in params.
void CSIMTOI::GetParameters(float * params, int size)
{
	mModelList->GetParameters(params, size);
}

/// Initialize SIMTOI
void CSIMTOI::Init(int argc, char *argv[])
{
	// Because the program is OpenGL + OpenCL we must init this way:
	// 1. Load modeling info (window size, scale)
	// 2. init OpenGL
	// 3. Load the model parameters, init the models.
	// 4. init OpenCL
	// 5.  Load OIFITS data
	// 6.  Init OpenCL Memory and routines

	// Load the current program configuration, then initialize OpenGL
	LoadConfiguration();
	mGL = new COpenGL(mWindow_width, mWindow_height, mScale, mShaderSourceDir);
	mGL->init(argc, argv);

	// Create the model list, load models into memory:
	mModelList = new CModelList();
	LoadModels();

	// Create the OpenCL Object, initialize
	mCL = new CLibOI();
	mCL->SetKernelSourcePath(mKernelSourceDir);
	mCL->Init(CL_DEVICE_TYPE_GPU, mWindow_width, mWindow_height, 1, mScale);
	mCL->RegisterImage_GLTB(mGL->GetFramebufferTexture());

	// Load OIFITS data into memory
	mCL->LoadData("/home/bkloppenborg/workspace/simtoi/bin/epsaur.oifits");

	// Now init memory and routines
	mCL->InitMemory();
	mCL->InitRoutines();
}

/// Loads current global parameters
void CSIMTOI::LoadConfiguration()
{
	// TODO: Read in the width, height, and scale from command line and/or config file
	mWindow_width = 128;
	mWindow_height = 128;
	mScale = 0.05;
	// TODO: We should determine the path to these dynamically, perhaps use paths relative to the
	// current executable inside of a try/catch block?
	mShaderSourceDir = "/home/bkloppenborg/workspace/simtoi/src/shaders";
	mKernelSourceDir = "/home/bkloppenborg/workspace/simtoi/lib/liboi/src/kernels";
}

void CSIMTOI::LoadModels()
{
    // TODO: Read this in from a text file.
	CShader * shader;
	shader = mGL->GetShader(LD_Hesteroffer1997);
	CModel * tmp = new CModelSphere();
	tmp->SetPositionType(XY);
	tmp->SetShader(shader);
	mModelList->Append(tmp);
}

/// Renders the model (all wavelengths) into the OpenGL framebuffer.
void CSIMTOI::Render()
{
	mModelList->Render(mGL);
}

/// Sets the model parameters, converting from a uniform hypercube to physical units as required.
void CSIMTOI::SetParameters(float * params, int n_params)
{
	mModelList->SetParameters(params, n_params);
}
