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
 *  Copyright (c) 2013 Brian Kloppenborg
 */

#ifndef CDENSITY_DISK_H_
#define CDENSITY_DISK_H_

#include "CModel.h"

class CDensityDisk: public CModel
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
	CDensityDisk();
	virtual ~CDensityDisk();

	void Init();

	void preRender(double & max_flux);
	void Render(const glm::mat4 & view, const GLfloat & max_flux);

	virtual void SetShader(CShaderPtr shader); // Overrides CModel::SetShader

};

#endif /* CDENSITY_DISK_H_ */
