/*
 * CShaderFactory.cpp
 *
 *  Created on: Feb 10, 2013
 *      Author: bkloppenborg
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

#include "CShaderFactory.h"
#include "CShader.h"
#include <stdexcept>

using namespace std;

// TODO: Instead of loading them explicitly here, it would be better to load them using plugins
//#include "GLShaders/CGLShaderSphere.h"

// The folder which contains the SIMTOI executable.
extern string EXE_FOLDER;

CShaderFactory::CShaderFactory()
{
	// TODO: Call the register function from a GUI class via a plugin-like script.
	Register(EXE_FOLDER + "/shaders/default.json");
	Register(EXE_FOLDER + "/shaders/ldl_claret2000.json");
	Register(EXE_FOLDER + "/shaders/ldl_logarithmic.json");
	Register(EXE_FOLDER + "/shaders/ldl_power_law.json");
	Register(EXE_FOLDER + "/shaders/ldl_quadratic.json");
	Register(EXE_FOLDER + "/shaders/ldl_square_root.json");
	Register(EXE_FOLDER + "/shaders/ldl_fields2003.json");
	Register(EXE_FOLDER + "/shaders/disk_pascucci2004.json");
	Register(EXE_FOLDER + "/shaders/disk_andrews2009.json");
	Register(EXE_FOLDER + "/shaders/disk_power_law.json");
	Register(EXE_FOLDER + "/shaders/texture_2d.json");
}

CShaderFactory::~CShaderFactory() \
{
	// TODO Auto-generated destructor stub
}

/// Create an instance of the specified GLShader inside of a CGLShaderWrapper object.
/// Returns a shared_ptr<CGLShaderWrapper> to the object if found, or throws a runtime exception.
shared_ptr<CShader> CShaderFactory::CreateShader(string GLShaderID)
{
	auto it = mFactory.find(GLShaderID);
	if(it != mFactory.end())
	{
		// Return a deep-copy of the shader.
		return shared_ptr<CShader>(new CShader(*it->second.get()));
	}

	throw runtime_error("The GLShader with ID '" + GLShaderID + "' not registered with CGLShaderFactory");

	return shared_ptr<CShader>();
}

/// Returns a vector of the GLShader names that are loaded.
vector<string> CShaderFactory::GetShaderList()
{
	vector<string> temp;

	for(auto it: mFactory)
		temp.push_back(it.first);

	return temp;
}

/// Returns a copy of the GLShaderFactory instance
CShaderFactory CShaderFactory::Instance()
{
	static CShaderFactory instance;
	return instance;
}

/// Loads a GLShader from the information specified in json_config_file
void CShaderFactory::Register(string json_config_file)
{
	CShaderPtr temp(new CShader(json_config_file));
	mFactory[temp->getID()] = temp;
}

/// Registers a GLShader with the name "GLShaderID" and creation function "CreateFunction" with the factory.
void CShaderFactory::Register(string shader_id, CShaderPtr shader)
{
	if(mFactory.find(shader_id) != mFactory.end())
		throw runtime_error("A GLShader with ID '" + shader_id + "' is already registered with CGLShaderFactory");

	mFactory[shader_id] = shader;
}
