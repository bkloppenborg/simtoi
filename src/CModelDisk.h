/*
 * CModelDisk.h
 *
 *  Created on: Feb 23, 2012
 *      Author: bkloppenborg
 *
 *  Implements a simple disk model which has two parameters: diameter and height.
 *  When rendered, this model is a cylinder.  Inheriting classes should call
 *  the CModelDisk(int) function and start numbering additional parameters at 3.
 */

#ifndef CMODELDISK_H_
#define CMODELDISK_H_

#include "CModel.h"

class CModelDisk : public CModel
{
protected:
	static int mDiskParams;
	int mSlices;
	int mStacks;
	double * mSinT;
	double * mCosT;
	double mZeroThreshold;

public:
	CModelDisk();
	CModelDisk(int additional_params);
	virtual ~CModelDisk();

	virtual void Draw();
	virtual void DrawDisk(double radius, double height);
	virtual void DrawSides(double radius, double height);

	virtual double GetRadius(double half_height, double h, double dh, double rim_radius);

	void InitMembers();

	void Render(GLuint framebuffer_object, int width, int height);

	virtual double Transparency(double half_height, double at_z);
};

#endif /* CMODELDISK_GAUSS_H_ */
