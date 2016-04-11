 /*
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
 *
 * Copyright (c) 2012 Brian Kloppenborg
 */

#ifndef CSHADER_H_
#define CSHADER_H_

#include "OpenGL.h" // OpenGL includes, plus several workarounds for various OSes

#include <string>
#include <vector>
#include <utility>

#include "CParameterMap.h"

using namespace std;

class CShader;

class CShader : public CParameterMap
{
protected:
	GLuint mProgram;
	GLuint mShader_vertex;
	GLuint mShader_fragment;
	vector<GLuint> mParam_locations;
	string mVertShaderFileName;
	string mFragShaderFilename;
	string mShader_dir;

	bool mShaderLoaded;

public:
	CShader(const CShader & other);
	CShader(string json_config_file);
	virtual ~CShader();

	void CompileShader(GLuint shader);

	GLuint GetProgram();

	void Init();

	void LinkProgram(GLuint program);

	void UseShader();
};

#endif /* CGLSHADER_H_ */
