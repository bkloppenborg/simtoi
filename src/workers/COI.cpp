/*
 * COI.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: bkloppenborg
 */

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

#include "COI.h"
#include <GL/gl.h>
#include <GL/glu.h>

COI::COI(CWorkerThread * WorkerThread)
	: CTask(WorkerThread)
{
	// First initialize local storage:
//	mImageWidth = mWorkerThread->GetImageWidth();
//	mImageHeight = mWorkerThread->GetImageHeight();
//	mImageScale = mWorkerThread->GetScale();

	InitFrameBuffers();

	// Initialize liboi using the worker thread's OpenCL+OpenGL context
//	mLibOI = new CLibOI()
//	mLibOI->SetImageSource(mFBO_storage_texture, LibOIEnums::OPENGL_TEXTUREBUFFER);
//	mLibOI->SetKernelSourcePath(mKernelSourceDir);
}

COI::~COI()
{
	delete mLibOI;

	glDeleteFramebuffers(1, &mFBO);
	glDeleteFramebuffers(1, &mFBO_texture);
	glDeleteFramebuffers(1, &mFBO_depth);
	glDeleteFramebuffers(1, &mFBO_storage);
	glDeleteFramebuffers(1, &mFBO_storage_texture);
}

string COI::GetDataDescription()
{
	return "OIFITS data";
}

vector<string> COI::GetDataTypes()
{
	vector<string> temp;
	temp.push_back("*.oifits");
	temp.push_back("*.fits");
	return temp;
}

unsigned int COI::GetNData()
{
	mLibOI->GetNData();
}

void COI::GetResiduals(valarray<double> & residuals)
{

}

void COI::GetUncertainties(valarray<double> & uncertainties)
{

}

void COI::InitFrameBuffers(void)
{
	InitMultisampleRenderBuffer();
	InitStorageBuffer();
}

void COI::InitMultisampleRenderBuffer(void)
{
	glGenFramebuffers(1, &mFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

	glGenRenderbuffers(1, &mFBO_texture);
	glBindRenderbuffer(GL_RENDERBUFFER, mFBO_texture);
	// Create a 2D multisample texture
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_RGBA32F, mImageWidth, mImageHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, mFBO_texture);

	glGenRenderbuffers(1, &mFBO_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, mFBO_depth);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH_COMPONENT, mImageWidth, mImageHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBO_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check that status of our generated frame buffer
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
    	const GLubyte * errStr = gluErrorString(status);
        printf("Couldn't create multisample frame buffer: %x %s\n", status, (char*)errStr);
        delete errStr;
        exit(0); // Exit the application
    }

    GLint bufs;
    GLint samples;
    glGetIntegerv(GL_SAMPLE_BUFFERS, &bufs);
    glGetIntegerv(GL_SAMPLES, &samples);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer
}

void COI::InitStorageBuffer(void)
{
    glGenTextures(1, &mFBO_storage_texture); // Generate one texture
    glBindTexture(GL_TEXTURE_2D, mFBO_storage_texture); // Bind the texture mFBOtexture

    // Create the texture in red channel only 8-bit (256 levels of gray) in GL_BYTE (CL_UNORM_INT8) format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mImageWidth, mImageHeight, 0, GL_RED, GL_FLOAT, NULL);
    // Enable this one for alpha blending:
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, NULL);
    // These other formats might work, check that GL_BYTE is still correct for the higher precision.
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, mWidth, mHeight, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32, mWidth, mHeight, 0, GL_RED, GL_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, mWidth, mHeight, 0, GL_RED, CL_HALF_FLOAT, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, mWidth, mHeight, 0, GL_RED, GL_FLOAT, NULL);


    // Setup the basic texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &mFBO_storage); // Generate one frame buffer and store the ID in mFBO
    glBindFramebuffer(GL_FRAMEBUFFER, mFBO_storage); // Bind our frame buffer

    // Attach the depth and texture buffer to the frame buffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mFBO_storage_texture, 0);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mFBO_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    // Check that status of our generated frame buffer
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
    	const GLubyte * errStr = gluErrorString(status);
        printf("Couldn't create storage frame buffer: %x %s\n", status, (char*)errStr);
        delete errStr;
        exit(0); // Exit the application
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind our frame buffer
}


void COI::LoadData(string filename)
{
	mLibOI->LoadData(filename);
}
