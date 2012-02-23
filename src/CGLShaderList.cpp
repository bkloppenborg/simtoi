/*
 * CGLShaderList.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
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

	// Simple scale height
	base_name = "ScaleHeight";
	friendly_name = "Scale Height - z";
	n_params = 1;
	param_names.clear();
	starting_values.clear();
	param_names.push_back("h_z");
	minmax.push_back(pair<float,float>(0.01, 1));
	starting_values.push_back(0.1);
	tmp = new CGLShader(CGLShaderList::SCALE_HEIGHT, shader_dir, base_name, friendly_name, n_params, param_names, starting_values, minmax);
	Append(tmp);
}
