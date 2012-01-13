/*
 * CGLShader.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 * A class for storing OpenGL Shader Objects (vertex + fragement) source code
 * and a link to the binary code once compiled on the OpenGL context.
 *
 * NOTE: Use a CGLShaderWrapper in CModel objects to save memory on the GPU.
 */

#ifndef CGLSHADER_H_
#define CGLSHADER_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <vector>

#include "CGLShaderList.h"

using namespace std;

class CGLShader
{
protected:
	GLuint mProgram;
	GLuint mShader_vertex;
	GLuint mShader_fragment;
	GLuint * mParam_locations;
	string mBase_name;
	string mShader_dir;
	string mFriendlyName;
	int mNParams;
	vector<string> mParam_names;

	eGLShaders mType;
	bool mShaderLoaded;

public:
	CGLShader(eGLShaders type, string shader_dir, string base_filename, string friendly_name, int n_parameters, vector<string> parameter_names);
	virtual ~CGLShader();

	eGLShaders GetType() { return mType; };
	string GetName() { return mFriendlyName; };
	int GetNParams() { return mNParams; }

	void Init();

	void UseShader(float * params, int in_params);

};

#endif /* CGLSHADER_H_ */
