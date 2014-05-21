/*
 * CRocheSpheroid.h
 *
 *  Created on: Apr 22, 2014
 *      Author: bkloppenborg
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

	bool mModelReady;

	// Quantities related to the pixel centroid and corners:
	vector<double> pixel_theta;
	vector<double> pixel_phi;
	vector<double> pixel_radii;
	vector<vec3>   pixel_xyz;
	unsigned int n_sides;
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
	CHealpixSpheroid(int n_params);
	virtual ~CHealpixSpheroid();

	void FindPixels(double s0, double s1, double s2,
			double ds0, double ds1, double ds2,
			vector<unsigned int> &pixels_ids);

	void GenerateHealpixSphere(unsigned int n_pixels, unsigned int n_sides);
	void GenerateVBO(unsigned int n_pixels, unsigned int n_side, vector<vec3> & vbo_data);
	void GenerateHealpixVBOIndicies(unsigned int n_pixels, vector<unsigned int> & elements);

	virtual void GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements) = 0;

	virtual void Render(GLuint framebuffer_object, const glm::mat4 & view) = 0;
	virtual void Init();

	void UploadVBO();
	void UploadEBO();
};

#endif /* CROCHESPHEROID_H_ */
