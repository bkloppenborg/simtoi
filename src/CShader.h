/*
 * CShader.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CSHADER_H_
#define CSHADER_H_

class CGLShader;

class CShader
{
protected:
	CGLShader * shader;
	float * params;
	int n_params;

public:
	CShader(CGLShader * shader, int n_params);
	virtual ~CShader();

	void GetParams(float * params, int n_params);
	int GetNFreeParams();

	void SetParams(float * params, int n_params);
	void UseShader();
};

#endif /* CSHADER_H_ */
