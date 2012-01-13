/*
 * CGLShaderList.h
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
 */

#ifndef CGLSHADERLIST_H_
#define CGLSHADERLIST_H_

#include <vector>
#include <string>
#include <utility>
#include "enumerations.h"

using namespace std;

class CGLShader;
class CGLShaderWrapper;

class CGLShaderList
{
protected:
	vector<CGLShader*> shaders;
	string shader_dir;
	bool SortModelPredicate(CGLShader * A, CGLShader * B);

public:
	CGLShaderList(string shader_dir);
	~CGLShaderList();

protected:
	void Append(CGLShader * CGLShader);

public:
	CGLShaderWrapper * GetShader(eGLShaders shader);
	vector< pair<eGLShaders, string> > GetShaderNames(void);

protected:
	CGLShader * FindShader(eGLShaders shader);

	void LoadShaders();
};

#endif /* CGLSHADERLIST_H_ */
