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

class COpenGLThread;
class CLibOI;
class CModelList;

using namespace std;

class CSIMTOI
{
protected:
	static CSIMTOI TheInstance;

protected:
	int mWindow_width;
	int mWindow_height;
	float mScale;
	string mShaderSourceDir;
	string mKernelSourceDir;

	// OpenGL, OpenCL Objects:

	// OpenGL/OpenCL Globals:
	COpenGLThread * mGL;
	CLibOI * mCL;

	// Model list:
	CModelList * mModelList;

public:
	CSIMTOI();
	virtual ~CSIMTOI();

	void BlitToScreen();

	float GetChi2(int data_num, int wl_num);
	int GetNFreeParameters();
	float GetLogLike(int data_num, int wl_num);
	void GetParameters(float * params, int size);

	void Init(int argc, char *argv[]);

	void LoadConfiguration();
	void LoadModels();

	void Render();

	void SetParameters(float * params, int n_params);

	static CSIMTOI * GetInstance() { return &TheInstance; };
};

#endif /* CSIMTOI_H_ */
