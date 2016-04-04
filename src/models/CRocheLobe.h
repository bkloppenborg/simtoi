/*
 * CRocheBinary.h
 *
 *  Created on: Feb 28, 2014
 *      Author: fbaron
 *  Description:
 *  The following class implements the equations for the Roche surface of an
 *  asynchronously rotating star under the assumption that the mass is centrally
 *  concentrated and uniformly rotating.
 *  voin Zeipel law is used to compute gravity darkening
 *
 */

#ifndef CROCHEBINARY_H_
#define CROCHEBINARY_H_

#include "CHealpixSpheroid.h"

class CRocheLobe: public CHealpixSpheroid
{
    public:
        CRocheLobe();
        virtual ~CRocheLobe();

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
            return "roche_lobe";
        };
        void GenerateModel(vector<vec3> & vbo_data, vector<unsigned int> & elements);

        void ComputeModel(double g_pole, double r_pole, double omega_rot);

        void VonZeipelTemperatures(double T_eff_pole, double g_pole, double beta);

        void ComputeGravity(const double r_pole, const double separation, const double q, const double P);
        void ComputeGravity(const double separation, const double q, const double P, 																		   const double radius, const double theta, const double phi, double & g_x, double & g_y, double & g_z, double & g_mag);

        void ComputeRadii(const double r_pole, const double separation, const double q, const double P);
        double ComputeRadius(const double polar_radius, const double separation, const double q,
                const double asynchronous_ratio, const double theta, const double phi);

        void ComputePotential(double & pot, double & dpot, const double radius, const double theta, const double phi,
                const double separation, const double q, const double asynchronous_ratio);


};

#endif /* CROCHEBINARY_H_ */
