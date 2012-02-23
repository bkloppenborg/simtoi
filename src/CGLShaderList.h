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

class CGLShaderList: public CVectorList<CGLShader*>
{
public:
	/// Enumerated shader names
	enum ShaderTypes
	{
		NONE = 0,
		LD_HESTEROFFER1997 = 1,
		SCALE_HEIGHT = 2,
		LAST_VALUE // must be the last element
	};

protected:
	string shader_dir;
	bool SortModelPredicate(CGLShader * A, CGLShader * B);

public:
	CGLShaderList(string shader_dir);
	~CGLShaderList();

public:
	CGLShaderWrapper * GetShader(ShaderTypes shader);
	vector< pair<ShaderTypes, string> > GetTypes(void);

protected:
	CGLShader * FindShader(ShaderTypes shader);

	void LoadShaders();
};

#endif /* CGLSHADERLIST_H_ */
