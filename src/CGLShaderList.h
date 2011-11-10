/*
 * CGLShaderList.h
 *
 *  Created on: Nov 9, 2011
 *      Author: bkloppenborg
 */

#ifndef CGLSHADERLIST_H_
#define CGLSHADERLIST_H_

#include <vector>
#include "enumerations.h"
#include "CGLShader.h"
#include "CShader.h"


class CGLShaderList
{
protected:
	vector<CGLShader*> shaders;

private:
	bool SortModelPredicate(CGLShader * A, CGLShader * B);

public:
	CGLShaderList();
	~CGLShaderList();

	void Append(CGLShader * feature);
	CShader * GetShader(eGLShaders shader);
	CGLShader * FindShader(eGLShaders shader);

protected:
	CGLShader * LoadShader(eGLShaders shader);

};

#endif /* CGLSHADERLIST_H_ */
