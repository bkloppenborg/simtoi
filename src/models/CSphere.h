/*
 * CSphere.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */
 
 /* 
 * Copyright (c) 2011 Brian Kloppenborg
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

#ifndef CSPHERE_H_
#define CSPHERE_H_

#include "CModel.h"

class CSphere: public CModel
{
protected:

	unsigned int mNumElements;

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	bool mModelReady;

public:
	CSphere();
	virtual ~CSphere();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "sphere"; };
	double GetMaxHeight();

	static void GenerateSphere_LatLon(vector<vec3> & vbo_data, vector<unsigned int> & elements,
			unsigned int latitude_subdivisions, unsigned int longitude_subdivisions);

	void Init();

	void Render(GLuint framebuffer_object, const glm::mat4 & view);
};

#endif /* CSPHERE_H_ */
