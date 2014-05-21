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
 
// Power law limb darkening implemented according to Hestroffer (1997)
// Implemented by decreasing the flux (color.x) of the vertex.

in vec3 Normal;
in vec2 Tex_Coords;

uniform float alpha;
uniform sampler2DRect TexSampler;

out vec4 out_color;

void main()
{
    vec4 Color = texture(TexSampler, Tex_Coords);
        
    float mu = abs(dot(Normal, vec3(0.0, 0.0, 1.0)));
    float intensity = pow(mu, alpha);

    out_color = vec4(intensity * Color.r, 0, 0, Color.a);
};