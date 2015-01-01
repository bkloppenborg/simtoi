/*
 * CDisk_ConcentricRings.h
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppenborg
 *
 */

#ifndef CDISK_CONCENTRIC_RINGS_H_
#define CDISK_CONCENTRIC_RINGS_H_

#include "CModel.h"

class CDisk_ConcentricRings: public CModel
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
	CDisk_ConcentricRings();
	virtual ~CDisk_ConcentricRings();

	static shared_ptr<CModel> Create();

	virtual string GetID() { return "disk_concentric_rings"; };

	void Init();

	void Render(const glm::mat4 & view);

	virtual void SetShader(CShaderPtr shader); // Overrides CModel::SetShader

};

#endif /* CDISK_CONCENTRIC_RINGS_H_ */
