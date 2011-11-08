/*
 * CModelSphere.h
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 */

#ifndef CMODELSPHERE_H_
#define CMODELSPHERE_H_

#include "CModel.h"

class CModelSphere: public CModel
{
public:
	CModelSphere();
	virtual ~CModelSphere();

	void Render(GLuint framebuffer_object, int width, int height);
};

#endif /* CMODELSPHERE_H_ */
