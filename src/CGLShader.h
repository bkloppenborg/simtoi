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
#include "enumerations.h"

using namespace std;

class CGLShader
{
protected:
	GLuint program;
	GLuint shader_vertex;
	GLuint shader_fragment;
	GLuint * param_locations;
	string base_name;
	string shader_dir;
	int n_params;
	vector<string> param_names;

	eGLShaders type;
	bool mShaderLoaded;

public:
	CGLShader(eGLShaders type, string shader_dir, string base_filename, int n_parameters, vector<string> parameter_names);
	virtual ~CGLShader();

	eGLShaders GetType();
	void Init();

	int GetNParams();

	void UseShader(float * params, int in_params);

};

#endif /* CGLSHADER_H_ */
