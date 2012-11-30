/*
 * CGLShaderList.h
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

#ifndef CGLSHADERLIST_H_
#define CGLSHADERLIST_H_

#include <vector>
#include <string>
#include <utility>
#include <memory>
#include <vector>
using namespace std;

class CGLShader;
class CGLShaderWrapper;

typedef shared_ptr<CGLShader> CGLShaderPtr;
typedef shared_ptr<CGLShaderWrapper> CGLShaderWrapperPtr;

class CGLShaderList: public vector<CGLShaderPtr>
{
public:
	/// Enumerated shader names
	enum ShaderTypes
	{
		NONE = 0,
		LDL_POWERLAW = 1,
		POWER_LAW_Z = 2,
		LDL_CLARET2000 = 3,
		LDL_SQUARE_ROOT = 4,
		LDL_QUADRATIC = 5,
		LDL_LOGARITHMIC = 6,
		LAST_VALUE // must be the last element
	};

protected:
	string shader_dir;
	bool SortModelPredicate(CGLShaderPtr A, CGLShaderPtr B);

public:
	CGLShaderList(string shader_dir);
	virtual ~CGLShaderList();

public:
	CGLShaderWrapperPtr GetShader(ShaderTypes shader);
	vector< pair<ShaderTypes, string> > GetTypes(void);

protected:
	CGLShaderPtr FindShader(ShaderTypes shader);

	void LoadShaders();
};

#endif /* CGLSHADERLIST_H_ */
