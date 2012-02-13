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
#include "CGLShader.h"

class CGLShaderWrapper : public CParameters
{
protected:
	CGLShader * mShader;

public:
	CGLShaderWrapper(CGLShader * shader, int n_params);
	virtual ~CGLShaderWrapper();

	CGLShaderList::ShaderTypes GetType() { return mShader->GetType(); };

	void UseShader();
};

#endif /* CGLSHADERWRAPPER_H_ */
