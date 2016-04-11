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

#ifndef CROCHEROTATOR_H_
#define CROCHEROTATOR_H_

#include "CHealpixSpheroid.h"

class CRocheRotator: public CHealpixSpheroid
{
public:
	CRocheRotator();
	virtual ~CRocheRotator();

public:

	void preRender(double & max_flux);
	void Render(const glm::mat4 & view, const GLfloat & max_flux);

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
