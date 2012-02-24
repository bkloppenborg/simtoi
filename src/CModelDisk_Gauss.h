/*
 * CModelDisk_Gauss.h
 *
 *  Created on: Feb 23, 2012
 *      Author: bkloppenborg
 *
 *  Implements a simple disk model in which the transparency as a function of height
 *  increases as exp(- abs(z_max - z) / h_z)
 */

#ifndef CMODELDISK_GAUSS_H_
#define CMODELDISK_GAUSS_H_

#include "CModel.h"

class CModelDisk_Gauss : public CModel
{
protected:
	int mSlices;
	int mStacks;
	double * mSinT;
	double * mCosT;

public:
	CModelDisk_Gauss();
	virtual ~CModelDisk_Gauss();

	void DrawDisk();
	void Render(GLuint framebuffer_object, int width, int height);
};

#endif /* CMODELDISK_GAUSS_H_ */
