#version 330 core

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
 * Copyright (c) 2012 Brian Kloppenborg
 */
 
// Four-parameter limb darkening implemented according to Claret (2003)
// Implemented by decreasing the flux (color.x) of the vertex.
in vec3 Normal;
in vec2 Tex_Coords;

uniform float a1;
uniform float a2;
uniform float a3; 
uniform float a4;
uniform sampler2DRect TexSampler;

out vec4 out_color;

void main(void)
{
    float mu = abs(dot(Normal, vec3(0.0, 0.0, 1.0)));
    
    // now compute the Claret 2003 limb darkening law.
    float intensity = 1;
	intensity -= a1 * (1 - pow(mu, 0.5));
	intensity -= a2 * (1 - mu);
	intensity -= a3 * (1 - pow(mu, 1.5));
	intensity -= a4 * (1 - pow(mu, 2));
    
    // look up the color from the texture.
    vec4 Color = texture(TexSampler, Tex_Coords);
    out_color = vec4(intensity * Color.x, 0, 0, Color.a);
}
