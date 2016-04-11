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

in vec3 position;
in vec3 normal;
in vec3 tex_coords;

uniform mat4 rotation;
uniform mat4 scale;
uniform mat4 translation;
uniform mat4 view;

out vec3 ModelPosition;
out vec3 Normal;
out vec2 Tex_Coords;

void main()
{
    Normal =  (rotation * vec4(normal, 0.0)).xyz;
    ModelPosition = (scale * vec4(position, 1.0)).xyz;
    gl_Position = view * translation * rotation * scale * vec4(position, 1.0);
    Tex_Coords = (tex_coords).xy;
}
