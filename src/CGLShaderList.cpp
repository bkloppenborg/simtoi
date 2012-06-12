/*
 * CGLShaderList.cpp
 *
 *  Created on: Nov 9, 2011
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

#include <algorithm>
#include <cstdio>
#include "CGLShader.h"
#include "CGLShaderList.h"
#include "CGLShaderWrapper.h"

CGLShaderList::CGLShaderList(string shader_source_dir)
{
	this->shader_dir = shader_source_dir;

	LoadShaders();
}

CGLShaderList::~CGLShaderList()
{

}

bool CGLShaderList::SortModelPredicate(CGLShader * A, CGLShader * B)
{
	if(A->GetType() < B->GetType())
		return true;

	return false;
}

/// Finds the specified shader in the list, returns the (compiled) version wrapped
/// into a CGLShaderWrapper.
CGLShaderWrapper * CGLShaderList::GetShader(ShaderTypes shader)
{
	// First see if the shader is already loaded
	CGLShader * tmp = NULL;
	tmp = FindShader(shader);

	CGLShaderWrapper * wrapper = NULL;
	if(tmp != NULL)
	{
		// Create the wrapper, init all data members to be free.
		wrapper = new CGLShaderWrapper(tmp, tmp->GetNParams());
		wrapper->SetAllFree(true);
	}

	return wrapper;
}

/// Returns a vector of pair<ShaderTypes, string> which identify the enumerated name and friendly name
/// of each shader in the list.
vector< pair<CGLShaderList::ShaderTypes, string> > CGLShaderList::GetTypes(void)
{
	vector< pair<ShaderTypes, string> > list;
	pair<ShaderTypes, string> tmp;

	// Iterate through the list and append the shader information
    for(vector<CGLShader*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	tmp.first = (*it)->GetType();
    	tmp.second = (*it)->GetName();

    	list.push_back(tmp);
    }

    return list;
}

/// Finds the specified shader.
CGLShader * CGLShaderList::FindShader(CGLShaderList::ShaderTypes shader)
{
	// There should be a small enough number of shaders that we can use find directly on the
	// unsorted vector list.  If this becomes a problem, we can sort the values later.
	if(shader < NONE || shader > LAST_VALUE)
		shader = NONE;

    for(vector<CGLShader*>::iterator it = mList.begin(); it != mList.end(); ++it)
    {
    	if((*it)->GetType() == shader)
    		return (*it);
    }

    return NULL;
}

/// Loads all avaliable shaders into memory
void CGLShaderList::LoadShaders()
{
	// TODO: Convert this to reading in a configuration file.
	string base_name;
	string friendly_name;
	int n_params;
	vector<string> param_names;
	CGLShader * tmp;
	vector<pair<float, float> > minmax;
	vector<float> starting_values;

	// Default do-nothing Shader
	base_name = "Default";
	friendly_name = "Default (None)";
	n_params = 0;
	param_names.clear();
	starting_values.clear();
	minmax.clear();
	tmp = new CGLShader(CGLShaderList::NONE, shader_dir, base_name, friendly_name, n_params, param_names, starting_values, minmax);
	Append(tmp);

	// Simple limb darkening.
	base_name = "LD_Hestroffer1997";
	friendly_name = "LDL - Hesteroffer";
	n_params = 1;
	param_names.clear();
	starting_values.clear();
	param_names.push_back("alpha");
	minmax.push_back(pair<float,float>(0.1, 1));
	starting_values.push_back(0.5);
	tmp = new CGLShader(CGLShaderList::LD_HESTEROFFER1997, shader_dir, base_name, friendly_name, n_params, param_names, starting_values, minmax);
	Append(tmp);

	// f(z) power law transparency
	base_name = "PowerLawZ";
	friendly_name = "Power Law Z";
	n_params = 1;
	param_names.clear();
	starting_values.clear();
	param_names.push_back("Beta");
	minmax.push_back(pair<float,float>(0.01, 1));
	starting_values.push_back(0.1);
	tmp = new CGLShader(CGLShaderList::POWER_LAW_Z, shader_dir, base_name, friendly_name, n_params, param_names, starting_values, minmax);
	Append(tmp);
}
