#version 120

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

// Simple scale height transparency.
varying out vec4 color;
varying out float transparency;

uniform vec3 min_xyz;
uniform vec3 max_xyz;

uniform float h_z;

void main(void)
{
    color = gl_Color;
    transparency = exp(-(max_xyz.z - abs(gl_Vertex.z))/h_z);
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
