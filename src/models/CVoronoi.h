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
 *  Copyright (c) 2014 Brian Kloppenborg
 */

#ifndef CVORONOI_H_
#define CVORONOI_H_

#include "CModel.h"

class CVoronoi: public CModel
{
protected:

	unsigned int mNRegions;
	unsigned int mNElements;

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

public:
	CVoronoi();
	virtual ~CVoronoi();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "voronoi"; };
	static void GenerateCone(vector<vec3> & vertices, vector<unsigned int> & elements);

	void Init();

	void Render(const glm::mat4 & view);

	virtual void SetShader(CShaderPtr shader); // Overrides CModel::SetShader

};

#endif /* CVORONOI_H_ */
