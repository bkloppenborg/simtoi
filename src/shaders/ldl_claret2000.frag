#version 120
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
 
// Four-parameter limb darkening implemented according to Claret (2003)
// Implemented by decreasing the flux (color.x) of the vertex.
in vec3 normal;
in vec4 color;
uniform float a1;
uniform float a2;
uniform float a3; 
uniform float a4;

void main(void)
{
    float mu = abs(dot(normal, vec3(0.0, 0.0, 1.0)));
    
    // now compute the Claret 2003 limb darkening law.
    float intensity = 1;
	intensity -= a1 * (1 - pow(mu, 0.5));
	intensity -= a2 * (1 - mu);
	intensity -= a3 * (1 - pow(mu, 1.5));
	intensity -= a4 * (1 - pow(mu, 2));

    gl_FragColor = vec4(intensity * color.x, 0, 0, color.w);
}
