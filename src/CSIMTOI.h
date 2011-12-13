/*
 * CSIMTOI.h
 *
 *  Created on: Dec 12, 2011
 *      Author: bkloppenborg
 *
 *  The singleton class implementing the primary functions in SIMTOI.
 *  This class serves as the only direct method to interface with the inner functions.
 */

#ifndef CSIMTOI_H_
#define CSIMTOI_H_

#include <string>

class COpenGL;
class CLibOI;
class CModelList;

using namespace std;

class CSIMTOI
{
private:
	// Globals
	int mWindow_width;
	int mWindow_height;
	float mScale;
	string mShaderSourceDir;
	string mKernelSourceDir;

	// OpenGL, OpenCL Objects:

	// OpenGL/OpenCL Globals:
	COpenGL * mGL;
	CLibOI * mCL;

	// Model list:
	CModelList * mModelList;

public:
	CSIMTOI(int argc, char *argv[]);
	virtual ~CSIMTOI();

	void BlitToScreen();

	float GetChi2(int data_num, int wl_num);
	float GetLogLike(int data_num, int wl_num);
	void GetParameters(float * params, int size);
protected:
	void Init(int argc, char *argv[]);

public:
	void LoadConfiguration();
	void LoadModels();

	void Render();

	void SetParameters(float * params, int n_params);
};

#endif /* CSIMTOI_H_ */
