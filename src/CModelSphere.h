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
	int slices;

public:
	CModelSphere();
	virtual ~CModelSphere();

	// CircleTable and DrawSphere come from OpenGLUT library.
	static void CircleTable( double **sint, double **cost, const int n );
	void DrawSphere( GLdouble radius, GLint slices, GLint stacks );

	void GetModelParameters(float * params, int n_params);
	int GetNModelFreeParameters();

	void Render(GLuint framebuffer_object, int width, int height);

	void SetModelParameters(float * params, int n_params);
};

#endif /* CMODELSPHERE_H_ */
