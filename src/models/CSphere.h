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
 *  Copyright (c) 2011 Brian Kloppenborg
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


public:
	CSphere();
	virtual ~CSphere();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "sphere"; };
	double GetMaxHeight();

	static void GenerateSphere_LatLon(vector<vec3> & vbo_data, vector<unsigned int> & elements,
			unsigned int latitude_subdivisions, unsigned int longitude_subdivisions);

	void Init();

	void preRender(double & max_flux);
	void Render(const glm::mat4 & view, const GLfloat & max_flux);
	void postRender();
};

#endif /* CSPHERE_H_ */
