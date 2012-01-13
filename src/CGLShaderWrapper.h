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

class CGLShader;

class CGLShaderWrapper
{
protected:
	CGLShader * shader;
	float * params;
	int n_params;

public:
	CGLShaderWrapper(CGLShader * shader, int n_params);
	virtual ~CGLShaderWrapper();

	void GetParams(float * params, int n_params);
	int GetNFreeParams();

	void SetParams(float * params, int n_params);
	void UseShader();
};

#endif /* CGLSHADERWRAPPER_H_ */
