/*
 * CCylinder.h
 *
 *  Created on: Feb 23, 2012
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
 * This file is part of the SImulation and Modeling Tool for Optical 
 * Interferometry (SIMTOI).
 * 
 * SIMTOI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License 
 * as published by the Free Software Foundation version 3.
 * 
 * SIMTOI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with SIMTOI.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 /*
 *  Implements a simple disk model which has two parameters: diameter and height.
 *  When rendered, this model is a cylinder.  Inheriting classes should call
 *  the CModelDisk(int) function and start numbering additional parameters at 3.
 */

#ifndef CCYLINDER_H_
#define CCYLINDER_H_

#include "CModel.h"

class CCylinder : public CModel
{
protected:
	static int mDiskParams;
	int mSlices;
	int mStacks;
	double * mSinT;
	double * mCosT;
	double mZeroThreshold;

public:
	CCylinder();
	CCylinder(int additional_params);
	virtual ~CCylinder();

	static shared_ptr<CModel> Create();

	virtual void Draw();
	virtual void DrawDisk(double radius, double at_z);
	virtual void DrawDisk(double r_in, double r_out, double at_z);
	virtual void DrawSides(double radius, double height);

	virtual string GetID() { return "cylinder"; };
	virtual double GetRadius(double half_height, double h, double dh, double rim_radius);

	void InitMembers();

	virtual double MidplaneColor(double radius) { return 1; };
	virtual double MidplaneTransparency(double radius) { return 1; };

	void Render(GLuint framebuffer_object, int width, int height);

	virtual double Transparency(double half_height, double at_z) { return 1; };
};

#endif /* CMODELDISK_GAUSS_H_ */
