#version 120
/* 
 * Copyright (c) 2013 Brian Kloppenborg
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
 
// A flux-conserving square-root limb darkening law based on Fields (2003)
// Implemented by decreasing the flux (color.x) of the vertex.
in vec3 normal;
in vec4 color;
uniform float A;
uniform float B;

void main(void)
{
    float mu = abs(dot(normal, vec3(0.0, 0.0, 1.0)));
    float intensity = 1;
    intensity -= A * (1 - 1.5*mu);
    intensity -= B * (1 - 2.5*sqrt(mu));

    gl_FragColor = vec4(intensity * color.x, 0, 0, color.w);
}
