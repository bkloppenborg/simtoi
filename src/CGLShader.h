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
#include <utility>

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
	unsigned int mNParams;
	vector<string> mParam_names;
	pair<float,float> * mMinMax;
	float * mStartingValues;

	CGLShaderList::ShaderTypes mType;
	bool mShaderLoaded;

public:
	CGLShader(CGLShaderList::ShaderTypes type, string shader_dir, string base_filename, string friendly_name, int n_parameters, vector<string> parameter_names, vector<float> starting_values, vector< pair<float, float> > minmax);
	virtual ~CGLShader();

	void CompileShader(GLuint shader);

	float GetMin(unsigned int i);
	float GetMax(unsigned int i);
	string GetName() { return mFriendlyName; };
	int GetNParams() { return mNParams; }
	string GetParamName(unsigned int i);
	float GetStartingValue(unsigned int i);
	CGLShaderList::ShaderTypes GetType() { return mType; };

	void Init();

	void LinkProgram(GLuint program);

	void UseShader(float * params, unsigned int in_params);

};

#endif /* CGLSHADER_H_ */
