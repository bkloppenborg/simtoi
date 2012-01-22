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
#include "CVectorList.h"

using namespace std;

class CGLShader;
class CGLShaderWrapper;

/// Enumerated shader names
enum eGLShaders
{
	SHDR_NONE,
	SHDR_LD_HESTEROFFER1997
};

class CGLShaderList: public CVectorList<CGLShader*>
{
protected:
	string shader_dir;
	bool SortModelPredicate(CGLShader * A, CGLShader * B);

public:
	CGLShaderList(string shader_dir);
	~CGLShaderList();

public:
	CGLShaderWrapper * GetShader(eGLShaders shader);
	vector< pair<eGLShaders, string> > GetShaderNames(void);

protected:
	CGLShader * FindShader(eGLShaders shader);

	void LoadShaders();
};

#endif /* CGLSHADERLIST_H_ */
