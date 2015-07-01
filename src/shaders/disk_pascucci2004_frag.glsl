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
 
// Pascucci disk model approximated by changing the transparency of the vertex.
in vec3 Normal;
in vec2 Tex_Coords;

in vec3 ModelPosition;

uniform float rho_0;
uniform float kappa;
uniform float r_d;
uniform float z_d;
uniform float alpha;
uniform float beta;
uniform float r_in;
uniform sampler2DRect TexSampler;

out vec4 out_color;

void main(void)
{
    // Compute the radius and height of this fragment
    float radius = sqrt(ModelPosition.x * ModelPosition.x + ModelPosition.y * ModelPosition.y);
    float height = atan(abs(ModelPosition.z)/radius);

    // Compute the density:
    // pi = 3.141592741012573
    float rho = rho0 * pow(radius/r_d, -alpha) * exp((-3.141592741012573/4.)*pow(height/z_d,2)*pow(radius/r_d,-2.*beta));
 
    // Compute the opacity:
    // Nothing to do - assume opacity is constant w.r.t. wavelength.
    
    // Compute the transparency
    float transparency = 1 - exp(-1 * kappa * rho);
    
    vec4 Color = texture(TexSampler, Tex_Coords);
    
    // If we are inside the inner radius, everything is transparent.
    if(radius - r_in < 1)
        out_color = vec4(0.0, 0.0, 0.0, 0.0);
    else
        out_color = vec4(Color.r, 0.0, 0.0, Color.a * transparency);
}
