/*
 * CShader.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 * A class for storing OpenGL Shader Objects (vertex + fragement) source code
 * and a link to the binary code once compiled on the OpenGL context.
 *
 * NOTE: Use a CGLShaderWrapper in CModel objects to save memory on the GPU.
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CSHADER_H_
#define CSHADER_H_

#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include <vector>
#include <utility>

#include "CParameters.h"

using namespace std;

class CShader;

class CShader : public CParameters
{
protected:
	GLuint mProgram;
	GLuint mShader_vertex;
	GLuint mShader_fragment;
	GLuint * mParam_locations;
	GLuint mMinXYZ_location;
	GLuint mMaxXYZ_location;
	string mBase_name;
	string mShader_dir;

	string mShaderID;

	bool mShaderLoaded;

public:
	CShader(const CShader & other);
	CShader(string json_config_file);
	CShader(string shader_id, string shader_dir, string base_filename, string friendly_name, int n_parameters, vector<string> parameter_names, vector<float> starting_values, vector< pair<float, float> > minmax);
	virtual ~CShader();

	void CompileShader(GLuint shader);

	string GetID() { return mShaderID; };

	void Init();

	void LinkProgram(GLuint program);

	void UseShader(double min_xyz[3], double max_xyz[3]);
	void UseShader(double min_xyz[3], double max_xyz[3], double * params, unsigned int in_params);

};

#endif /* CGLSHADER_H_ */
