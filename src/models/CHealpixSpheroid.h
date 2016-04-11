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

#ifndef CROCHESPHEROID_H_
#define CROCHESPHEROID_H_

#include "chealpix.h"
#include "CModel.h"

class CHealpixSpheroid : public CModel
{
protected:
	GLuint mVAO;
	GLuint mVBO;
	GLuint mEBO;

	vector<vec3> mVBOData;
	vector<unsigned int> mElements;

	// Quantities related to the pixel centroid and corners:
	vector<double> pixel_theta;
	vector<double> pixel_phi;
	vector<double> pixel_radii;
	vector<vec3>   pixel_xyz;
	unsigned int n_pixels;

	vector<double> corner_theta;
	vector<double> corner_phi;
	vector<double> corner_radii;
	vector<vec3>   corner_xyz;

	vector<double> gravity; // gravity intensity (not gravity vector)
	vector<double> g_x;
	vector<double> g_y;
	vector<double> g_z;

public:
	CHealpixSpheroid();
	virtual ~CHealpixSpheroid();

	void FindPixels(double s0, double s1, double s2,
			double ds0, double ds1, double ds2,
			vector<unsigned int> &pixels_ids);

	void GenerateHealpixSphere(unsigned int n_pixels, unsigned int n_sides);
	void GenerateVBO(unsigned int n_pixels, unsigned int n_side, vector<vec3> & vbo_data);
	void GenerateHealpixVBOIndicies(unsigned int n_pixels, vector<unsigned int> & elements);

	virtual void GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements) = 0;

	void preRender(double & max_flux) = 0;
	void Render(const glm::mat4 & view, const GLfloat & max_flux) = 0;
	virtual void Init();

	void UploadVBO();
	void UploadEBO();
};

#endif /* CROCHESPHEROID_H_ */
