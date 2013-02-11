/*
 * CGLShaderWrapper.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  A wrapper for a CGLShader that stores parameter values for use in CModel objects.
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

#ifndef CGLSHADERWRAPPER_H_
#define CGLSHADERWRAPPER_H_

#include <memory>
using namespace std;

#include "CParameters.h"
#include "CShader.h"

typedef shared_ptr<CShader> CGLShaderPtr;

class CGLShaderWrapper : public CParameters
{
protected:
	CGLShaderPtr mShader;

public:
	CGLShaderWrapper(CGLShaderPtr shader, int n_params);
	virtual ~CGLShaderWrapper();

	void UseShader(double min_xyz[3], double max_xyz[3]);
};

#endif /* CGLSHADERWRAPPER_H_ */
