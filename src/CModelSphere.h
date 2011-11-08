/*
 * CModelSphere.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CMODELSPHERE_H_
#define CMODELSPHERE_H_

#include "CModel.h"

class CModelSphere: public CModel
{
protected:
	double radius;
	int slices;

public:
	CModelSphere();
	virtual ~CModelSphere();

	int GetNModelFreeParameters();
	void Render(GLuint framebuffer_object, int width, int height);

	void SetModelParameters(double * params, int n_params);
};

#endif /* CMODELSPHERE_H_ */
