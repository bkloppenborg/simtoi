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
 
// Square root limb darkening
// Implemented by decreasing the flux (color.x) of the vertex.

in vec3 Normal;
in vec2 Tex_Coords;

uniform float a1;
uniform float a2;
uniform sampler2DRect TexSampler;

void main(void)
{
    float mu = abs(dot(Normal, vec3(0.0, 0.0, 1.0)));
    
    // Simple quadratic limb darkening:
    float intensity = 1;
	intensity -= a1 * (1 - mu);
	intensity -= a2 * (1 - sqrt(mu));

    vec4 Color = texture(TexSampler, Tex_Coords);
    gl_FragColor = vec4(intensity * Color.x, 0, 0, Color.a);
}
