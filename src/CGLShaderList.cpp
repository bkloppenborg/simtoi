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
	// Deallocate the shaders
	for(int i = shaders.size() - 1; i > -1; i--)
		if(shaders[i]) delete shaders[i];
}

bool CGLShaderList::SortModelPredicate(CGLShader * A, CGLShader * B)
{
	if(A->GetType() < B->GetType())
		return true;

	return false;
}

void CGLShaderList::Append(CGLShader * shader)
{
	shaders.push_back(shader);
}

/// Finds the specified shader in the list, returns the (compiled) version wrapped
/// into a CGLShaderWrapper.
CGLShaderWrapper * CGLShaderList::GetShader(eGLShaders shader)
{
	// First see if the shader is already loaded
	CGLShader * tmp;
	tmp = FindShader(shader);

	if(tmp != NULL)
		return new CGLShaderWrapper(tmp, tmp->GetNParams());

	// Otherwise throw an exception
	printf("Could not find shader, aborting!");
	throw;
}

/// Returns a vector of pair<eGLShaders, string> which identify the enumerated name and friendly name
/// of each shader in the list.
vector< pair<eGLShaders, string> > CGLShaderList::GetShaderNames(void)
{
	vector< pair<eGLShaders, string> > list;
	pair<eGLShaders, string> tmp;

	// Iterate through the list and append the shader information
    for(vector<CGLShader*>::iterator it = shaders.begin(); it != shaders.end(); ++it)
    {
    	tmp.first = (*it)->GetType();
    	tmp.second = (*it)->GetName();

    	list.push_back(tmp);
    }

    return list;
}

/// Finds the specified shader.
CGLShader * CGLShaderList::FindShader(eGLShaders shader)
{
	// There should be a small enough number of shaders that we can use find directly on the
	// unsorted vector list.  If this becomes a problem, we can sort the values later.
    for(vector<CGLShader*>::iterator it = shaders.begin(); it != shaders.end(); ++it)
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

	// Simple limb darkening.
	base_name = "LD_Hestroffer1997";
	friendly_name = "LDL - Hesteroffer (1997)";
	n_params = 1;
	param_names.push_back("alpha");
	tmp = new CGLShader(SHDR_LD_HESTEROFFER1997, shader_dir, base_name, friendly_name, n_params, param_names);
	this->Append(tmp);
}
