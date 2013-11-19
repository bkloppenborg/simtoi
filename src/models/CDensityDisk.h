/*
 * CDensityDisk.h
 *
 *  Created on: Nov. 5, 2013
 *      Author: bkloppenborg
 *
 *  This is the base class for disk models which come from a physically
 *  motivated density distribution (unlike those inheriting from CCylinder).
 *
 *  At present, models are rendered as a series of partially opaque concentric
 *  rings oriented along the z-direction. The rings are connected at the
 *  midplane by a triangle fan. This rendering method is suitable for disks
 *  seen edge-on, but probably not for other inclinations.
 *
 *  This base class has four parameters:
 *  	r_in : the disk model's inner radius
 *  	r_cutoff : the radius at which rendering will stop
 *  	h_cutoff : the height (z) at which rendering will stop
 *  	n_rings : the number of rings in the model
 *  The radial and height cutoffs should be defined to be sufficiently large
 *  as to not cut off meaningful density distribution. This requires some
 *  manual tweaking by the user.
 *
 */

#ifndef CDENSITY_DISK_H_
#define CDENSITY_DISK_H_

#include "CModel.h"

class CDensityDisk: public CModel
{
	protected:

	unsigned int mNumElements;

	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	bool mModelReady;

public:
	CDensityDisk(int n_additional_params);
	virtual ~CDensityDisk();

//	virtual double Density(double radius, double height) = 0;

//	void DrawDisk(double r_in, double r_out);
//	void DrawSide(double radius);

	static void GenreateCylinderRim(vector<vec3> & vertices, vector<unsigned int> & elements,
			unsigned int z_divisions, unsigned int phi_divisions);

	void Init();

//	virtual double Opacity(double radius, double height, double wavelength) = 0;

	void Render(GLuint framebuffer_object, const glm::mat4 & view);

//	virtual double Transparency(double radius, double height, double wavelength);

	virtual void SetShader(CShaderPtr shader); // Overrides CModel::SetShader

};

#endif /* CDENSITY_DISK_H_ */
