/*
 * CGLShaderList.cpp
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
 */

#include <algorithm>

#include "CGLShaderList.h"
#include "CGLShader.h"

CGLShaderList::CGLShaderList(string shader_dir)
{
	this->shader_dir = shader_dir;

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

CShader * CGLShaderList::GetShader(eGLShaders shader)
{
	// First see if the shader is already loaded
	CGLShader * tmp;
	tmp = FindShader(shader);

	if(tmp != NULL)
		return new CShader(tmp, tmp->GetNParams());

	// Otherwise load the shader, append it to the list of shaders and return the object.
	tmp = LoadShader(shader);
	Append(tmp);

	return new CShader(tmp, tmp->GetNParams());
}

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

// Loads a shader from source, returns it as a pointer.
CGLShader * CGLShaderList::LoadShader(eGLShaders shader)
{
	CGLShader * tmp;
	string base_name;
	int n_params;
	vector<string> param_names;

	// NOTE: If new shaders are added, they need to be specified here before they will work.
	if(shader == LD_Hesteroffer1997)
	{
		base_name = "LD_Hestroffer1997";
		n_params = 1;
		param_names.push_back("alpha");
		tmp = new CGLShader(shader, shader_dir, base_name, n_params, param_names);
	}

	if(tmp != NULL)
		return tmp;

	throw "Cannot load shader!";
}
