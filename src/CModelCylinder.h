/*
 * CModelCylinder.h
 *
 *  Created on: Jan 19, 2012
 *      Author: bkloppenborg
 *
 *  A basic class for drawing a solid cylinder (ends plus walls).
 */

#ifndef CMODELCYLINDER_H_
#define CMODELCYLINDER_H_

#include "CModel.h"

class CModelCylinder: public CModel
{
protected:
	int mSlices;
	int mStacks;
	double * mSinT;
	double * mCosT;

public:
	CModelCylinder();
	virtual ~CModelCylinder();

	static void CircleTable( double *sint, double *cost, const int n );

	void DrawCylinder();

	void Render(GLuint framebuffer_object, int width, int height);
};

#endif /* CMODELCYLINDER_H_ */
