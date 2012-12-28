/*
 * CModelDisk_ConcentricRings.h
 *
 *  Created on: Dec 28, 2012
 *      Author: bkloppen
 *
 *  A class implementing a disk model consisting of a series of partially
 *  opaque concentric rings.
 *
 *  The model has the following parameters:
 *  	r_in : the inner radius (>= 0)
 *  	r_out: the outer radius (> 0)
 *  	alpha: the radial power law decay coefficient for midplane transparency
 *  	beta : the z power law decay coefficient for transparency
 *  	n_rings: the number of concentric rings between r_in and r_out (> 0)
 */

#ifndef CMODELDISK_CONCENTRICRINGS_H_
#define CMODELDISK_CONCENTRICRINGS_H_

#include "CModelDisk.h"

class CModelDisk_ConcentricRings: public CModelDisk
{

public:
	CModelDisk_ConcentricRings();
	virtual ~CModelDisk_ConcentricRings();

	void Render(GLuint framebuffer_object, int width, int height);

	virtual void SetShader(CGLShaderWrapperPtr shader); // Overrides CModel::SetShader

};

#endif /* CMODELDISK_CONCENTRICRINGS_H_ */
