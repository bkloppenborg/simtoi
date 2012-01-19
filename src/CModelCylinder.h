/*
 * CModelCylinder.h
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 *
 *  A basic class for drawing a cylinder (cylindrical wall plus ends).
 */

#ifndef CMODELCYLINDER_H_
#define CMODELCYLINDER_H_

#include "CModel.h"

class CModelCylinder: public CModel
{
protected:
	int mSlices;
	int mStacks;

public:
	CModelCylinder();
	virtual ~CModelCylinder();

	void Render(GLuint framebuffer_object, int width, int height);
};

#endif /* CMODELCYLINDER_H_ */
