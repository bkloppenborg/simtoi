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
 *  Copyright (c) 2014 Fabien Baron
 */

#ifndef CROCHEBINARYFF_H_
#define CROCHEBINARYFF_H_

#include "CHealpixSpheroid.h"

class CRocheLobe_FF: public CHealpixSpheroid
{
    public:
        CRocheLobe_FF();
        virtual ~CRocheLobe_FF();

    protected:
        // Constants (initialized in constructor for GCC 4.6 compatability)
        const double AU;	// meters
        const double rsun;	// m
        const double G;		// m3 kg-1 s-2
        const double parsec;// m

    public:

        void preRender(double & max_flux);
        void Render(const glm::mat4 & view, const GLfloat & max_flux);

        static shared_ptr<CModel> Create();

        virtual string GetID()
        {
            return "roche_lobe_FF";
        };
        void GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements);

        void ComputeModel(double g_pole, double r_pole, double omega_rot);

        void VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta);
        double ComputeRL1(const double q, const double P);
        void ComputeGravity(const double separation, const double q, const double P);
        void ComputeGravity(const double separation, const double q, const double P, const double radius, const double theta, const double phi, double & g_x, double & g_y, double & g_z, double & g_mag);

        void ComputeRadii(const double pot_surface, const double separation, const double q, const double P);
        double ComputeRadius(const double pot_surface, const double separation, const double q, const double asynchronous_ratio, const double theta, const double phi);

        void ComputePotential(double & pot, double & dpot, const double radius, const double theta, const double phi, const double separation, const double q, const double asynchronous_ratio);
};

#endif /* CROCHEBINARYFF_H_ */
