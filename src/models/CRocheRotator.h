/*
 * CRocheRotator.h
 *
 *  Created on: Apr. 21, 2014
 *      Author: bkloppenborg
 *
 *  Description:
 *  The following class implements the equations for the Roche surface of a
 *  rapidly rotating star under the assumption that the mass is centrally
 *  concentrated and uniformly rotating. This code follows the equations
 *  originally specified in
 *  	Cranmer, S. R., & Owocki, S. P. (1995)
 *  	The Astrophysical Journal, 440, 308. doi:10.1086/175272
 *  and
 *  	Aufdenberg, J. P., Merand, A., Foresto, V. C. et al. (2006)
 *  	The Astrophysical Journal, 645(1), 664–675. doi:10.1086/504149
 *  to implement the gravity and temperature profiles respectively.
 */

#ifndef CROCHEROTATOR_H_
#define CROCHEROTATOR_H_

#include "CHealpixSpheroid.h"

class CRocheRotator: public CHealpixSpheroid
{
public:
	CRocheRotator();
	virtual ~CRocheRotator();

private:
	double lambda;
	double mTime;

public:

	void Render(const glm::mat4 & view);

	static shared_ptr<CModel> Create();

	virtual string GetID()
	{
		return "roche_rotator";
	};

	void GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements);

	void ComputeModel(double g_pole, double r_pole, double omega_rot);

	void ComputeRadii(double r_pole, double omega_rot);
	double ComputeRadius(double polar_radius, double omega_rot, double theta);

	void ComputeGravity(double g_pole, double r_pole, double omega_rot);
	void ComputeGravity(double g_pole, double r_pole, double omega,
			double radius_at_theta, double theta, double phi,
			double & g_x, double & g_y, double & g_z, double & g_mag);

	void VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta);

};

#endif /* CROCHEROTATOR_H_ */
