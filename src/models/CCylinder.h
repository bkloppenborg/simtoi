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
 *  When rendered, this model is a cylinder. The cylinder is centered on the x-z plane
 *  and extends in the y-direction.
 *
 *  Inheriting classes should call the CModelDisk(int) function and start numbering
 *  additional parameters at 3.
 */

#ifndef CCYLINDER_H_
#define CCYLINDER_H_

#include "CModel.h"

class CCylinder : public CModel
{
protected:

	unsigned int mRimStart;
	unsigned int mRimSize;
	unsigned int mMidplaneStart;
	unsigned int mMidplaneSize;

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	bool mModelReady;

public:
	CCylinder();
	virtual ~CCylinder();

	static shared_ptr<CModel> Create();

	static void GenerateMidplane(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int r_divisions, unsigned int phi_divisions);

	static void GenerateMidplane(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int element_offset,
			unsigned int r_divisions, unsigned int phi_divisions);

	/// Draws a unit cylindrical wall in the z-direction from (z = -0.5 ... 0.5, at r = 1)
	static void GenreateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int vertex_offset,
			unsigned int z_divisions, unsigned int phi_divisions);

	static void GenreateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int z_divisions, unsigned int phi_divisions);

	void Init();

	void Render(GLuint framebuffer_object, const glm::mat4 & view);
};

#endif /* CMODELDISK_GAUSS_H_ */
