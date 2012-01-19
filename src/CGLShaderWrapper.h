/*
 * CGLShaderWrapper.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 *  A wrapper for a CGLShader that stores parameter values for use in CModel objects.
 */

#ifndef CGLSHADERWRAPPER_H_
#define CGLSHADERWRAPPER_H_

#include "CParameters.h"

class CGLShader;

class CGLShaderWrapper : public CParameters
{
protected:
	CGLShader * mShader;

public:
	CGLShaderWrapper(CGLShader * shader, int n_params);
	virtual ~CGLShaderWrapper();

	void UseShader();
};

#endif /* CGLSHADERWRAPPER_H_ */
