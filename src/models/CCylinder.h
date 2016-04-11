 /*
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
 *
 *  Copyright (c) 2012 Brian Kloppenborg
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
	static void GenerateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int vertex_offset,
			unsigned int z_divisions, unsigned int phi_divisions);

	static void GenerateRim(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int z_divisions, unsigned int phi_divisions);

	void Init();

	void preRender(double & max_flux);
	void Render(const glm::mat4 & view, const GLfloat & max_flux);
};

#endif /* CMODELDISK_GAUSS_H_ */
