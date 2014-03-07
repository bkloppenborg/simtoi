#version 150 core
/* 
 * Copyright (c) 2012 Brian Kloppenborg
 *
 * If you use this software as part of a scientific publication, please cite as:
 *
 * Kloppenborg, B.; Baron, F. (2012), "SIMTOI: The SImulation and Modeling 
 * Tool for Optical Interferometry" (Version X). 
 * Available from  <https://github.com/bkloppenborg/simtoi>.
 *
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
 */
 
// Square root limb darkening
// Implemented by decreasing the flux (color.x) of the vertex.
in vec3 Normal;
in vec2 Color;
in vec3 ModelPosition;

uniform float rho0;
uniform float kappa;
uniform float r0;
uniform float h0;
uniform float alpha;
uniform float beta;

out vec4 out_color;

void main(void)
{
    // Compute the radius and height of this fragment
    float radius = sqrt(ModelPosition.x * ModelPosition.x + ModelPosition.y * ModelPosition.y);
    float height = abs(ModelPosition.z);

    // Compute the density:
    float midplane_density = rho0 * pow(radius / r0, -alpha);
    float height_scaling = -0.5 * pow(height / (h0 * pow(radius / r0, beta)), 2);
    float rho = rho0 * pow(radius / r0, -alpha) * exp(height_scaling);
 
    // Compute the opacity:
    // Nothing to do - assume opacity is constant w.r.t. wavelength.
    
    // Compute the transparency
    float transparency = 1 - exp(-1 * kappa * rho);
    
    out_color = vec4(Color.x, 0.0, 0.0, Color.y * transparency);
}
