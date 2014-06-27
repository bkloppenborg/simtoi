/*
 * CVoronoi.h
 *
 *  Created on: Jan. 10, 2014
 *      Author: bkloppenborg
 *
 *
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

	bool mModelReady;

public:
	CVoronoi();
	virtual ~CVoronoi();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "voronoi"; };
	static void GenerateCone(vector<vec3> & vertices, vector<unsigned int> & elements);

	void Init();

	void Render(GLuint framebuffer_object, const glm::mat4 & view);

	virtual void SetShader(CShaderPtr shader); // Overrides CModel::SetShader

};

#endif /* CVORONOI_H_ */
