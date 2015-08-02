#version 330 core
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
 
// 

in vec3 Normal;
in vec2 Tex_Coords;

in vec3 ModelPosition;

uniform bool use_r_trans;
uniform float r_in;
uniform float r_max;
uniform float alpha_r;

uniform bool use_z_trans;
uniform float z_max;
uniform float beta_z;

uniform sampler2DRect TexSampler;

out vec4 out_color;

void main(void)
{
    // Compute the radius and height of this fragment
    float radius = sqrt(ModelPosition.x * ModelPosition.x + ModelPosition.y * ModelPosition.y);
    float z = abs(ModelPosition.z);
    
    float r_trans = 1;
    float z_trans = 1;
    
    if(use_r_trans)
        r_trans -= pow(radius / r_max, alpha_r);
    if(use_z_trans)
        z_trans -= pow(z / z_max, beta_z);

    // Compute the transparency
    float transparency = r_trans * z_trans;
    
    // compute the output color
    vec4 Color = texture(TexSampler, Tex_Coords);
    if(radius - r_in < 0)
        out_color = vec4(0.0, 0.0, 0.0, 0.0);
    else
        out_color = vec4(Color.r, 0.0, 0.0, Color.a * transparency);
}
