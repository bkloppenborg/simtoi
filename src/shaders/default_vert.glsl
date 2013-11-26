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
 
in vec3 position;
in vec3 normal;
in vec2 vert_color;

uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 translation;
uniform mat4 view;
uniform vec2 uni_color = vec2(1.0, 1.0);// init to reasonable default

uniform bool color_from_uniform = true;

out vec3 Normal;
out vec2 Color;
out vec3 ModelPosition;

void main() 
{
	// The color can be either from a VBO or from a uniform, let the user decide.
	if(color_from_uniform)
		Color = uni_color;
	else
		Color = vert_color;
	
	// Rotate the normal vectors to match the orientation of the model
	Normal =  (rotation * vec4(normal, 0.0)).xyz;

	// Compute the positions of the vertices.
	ModelPosition = (scale * vec4(position, 1.0)).xyz;
    gl_Position = view * translation * rotation * scale * vec4(position, 1.0);
}
