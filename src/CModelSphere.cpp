/*
 * CModelSphere.cpp
 *
 *  Created on: Nov 8, 2011
 *      Author: bkloppenborg
 *
 */

/* --------
 * This code makes use of the glutSolidSphere rendering routine in DrawSphere,
 * Copyright notice for OpenGLUT follows:
 * --------
 *
 * OpenGLUT geometry rendering methods.
 *
 * Portions copyright (C) 2004, the OpenGLUT project contributors.
 * OpenGLUT branched from freeglut in February, 2004.
 *
 * Copyright (c) 1999-2000 Pawel W. Olszta. All Rights Reserved.
 * Written by Pawel W. Olszta, <olszta@sourceforge.net>
 * Creation date: Fri Dec 3 1999
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAWEL W. OLSZTA BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CModelSphere.h"
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <float.h>

using namespace std;

CModelSphere::CModelSphere()
	: CModel(1)
{
	// CModel(1) because we have one additional parameter for this model
	// Remember, mParams[0] = yaw, mParams[1] = pitch, mParams[2] = roll.

	mSlices = 50;	// seems like a good number.
	mName = "Sphere";
	mType = MDL_SPHERE;

	// Initalize the parameters to some useful value:
	mParamNames.push_back("Radius");
	mParams[3] = 1.0;
	mFreeParams[3] = true;

}

CModelSphere::~CModelSphere()
{
	// TODO Auto-generated destructor stub
}

/*!
    \brief Compute a lookup table of cos and sin values forming a cirle.
    It is the responsibility of the caller to free these tables
    The size of the table is (n+1) to form a connected loop
    The last entry is exactly the same as the first
    The sign of n can be flipped to get the reverse loop
*/
void CModelSphere::CircleTable( double **sint, double **cost, const int n )
{
	// Code modified from OpenGLUT,
	// http://sourceforge.net/projects/openglut/
    int i;
    const int size = abs( n );
    double angle;

    assert( n );
    angle = 2 * M_PI / ( double )n;

    *sint = ( double * )calloc( sizeof( double ), size + 1 );
    *cost = ( double * )calloc( sizeof( double ), size + 1 );

    if( !( *sint ) || !( *cost ) )
    {
        free( *sint );
        free( *cost );
        printf( "Failed to allocate memory in ogCircleTable" );
    }

    for( i = 0; i < size; i++ )
    {
        ( *sint )[ i ] = sin( angle * i );
        ( *cost )[ i ] = cos( angle * i );
    }

    /* Last sample is duplicate of the first */
    ( *sint )[ size ] = ( *sint )[ 0 ];
    ( *cost )[ size ] = ( *cost )[ 0 ];
}

/// Draws a sphere of the specified radius by making n-slices and m-stacks.
void CModelSphere::DrawSphere( GLdouble radius, GLint slices, GLint stacks )
{
	// Code modified from OpenGLUT,
	// http://sourceforge.net/projects/openglut/
    int i, j;

    /* Adjust z and radius as stacks are drawn. */
    double z0, z1;
    double r0, r1;

    /* Pre-computed circle */
    double *sint1 = NULL, *cost1 = NULL;
    double *sint2 = NULL, *cost2 = NULL;

    if( fabs(radius) < DBL_EPSILON )
    {
        printf( "Near-zero radius in DrawSphere" );
        return;
    }

    if( slices == 0 )
    {
    	printf( "Zero slices in DrawSphere" );
        return;
    }

    if( stacks == 0 )
    {
    	printf( "Zero stacks in DrawSphere" );
        return;
    }

    CircleTable( &sint1, &cost1, -slices    );
    CircleTable( &sint2, &cost2, stacks * 2 );

    slices = abs( slices );
    stacks = abs( stacks );

    if( sint1 && cost1 && sint2 && cost2 )
    {
        /* The top stack is covered with a triangle fan */
        z0 = 1.0;
        z1 = cost2[ 1 ];
        r0 = 0.0;
        r1 = sint2[ 1 ];

        glBegin( GL_TRIANGLE_FAN );
        glNormal3d( 0, 0, 1 );
        glVertex3d( 0, 0, radius );

        for( j = slices; j >= 0; j-- )
        {
            glNormal3d( cost1[ j ] * r1, sint1[ j ] * r1, z1 );
            glVertex3d(
                cost1[ j ] * r1 * radius, sint1[ j ] * r1 * radius, z1 * radius
            );
        }
        glEnd( );

        /* Cover each stack with a quad strip except the top/bottom stacks */
        for( i = 1; i < stacks - 1; i++ )
        {
            z0 = z1;
            z1 = cost2[ i + 1 ];
            r0 = r1;
            r1 = sint2[ i + 1];

            glBegin( GL_QUAD_STRIP );
            for( j = 0; j <= slices; j++ )
            {
                glNormal3d( cost1[ j ] * r1, sint1[ j ] * r1, z1 );
                glVertex3d(
                    cost1[ j ] * r1 * radius,
                    sint1[ j ] * r1 * radius,
                    z1 * radius
                );
                glNormal3d( cost1[ j ] * r0, sint1[ j ] * r0, z0 );
                glVertex3d(
                    cost1[ j ] * r0 * radius,
                    sint1[ j ] * r0 * radius,
                    z0 * radius
                );
            }
            glEnd( );
        }

        /* The bottom stack is covered with a triangle fan */
        z0 = z1;
        r0 = r1;

        glBegin( GL_TRIANGLE_FAN );
        glNormal3d( 0, 0, -1 );
        glVertex3d( 0, 0, -radius );

        for(j = 0; j <= slices; j++ )
        {
            glNormal3d( cost1[ j ] * r0, sint1[ j ] * r0, z0 );
            glVertex3d(
                cost1[ j ] * r0 * radius, sint1[ j ] * r0 * radius, z0 * radius
            );
        }
        glEnd( );
    }

    free( sint1 );
    free( cost1 );
    free( sint2 );
    free( cost2 );
}


void CModelSphere::Render(GLuint framebuffer_object, int width, int height)
{
	// NOTE: When rendering assume that the framebuffer has already been cleared.

	// Rename a few variables for convenience:
	float radius = mParams[3];

	// Bind to the framebuffer and draw the sphere.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object);

	glPushMatrix();
		// All models should load the modelview and identity matrix after a glPushMatrix().
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// Use the shader (this is valid to call even if a shader hasn't been assigned to this object).
		UseShader();

		// Call base-class rotation and translation functions.
		// NOTE: OpenGL applies these operations in a stack-like buffer so they are reversed
		// compared to conventional application.
		Translate();
		Rotate();

		// Model defined drawing functions:
		DrawSphere(radius, mSlices, mSlices);
	glPopMatrix();

	// Return to the default framebuffer before leaving.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
