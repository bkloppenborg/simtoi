/*
 * COpenGLThread.h
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 *
 *
 *  A singleton class.  Object responsible for all OpenGL rendering operations
 *  and storage of memory objects used by SIMTOI.
 *

 */

#ifndef COPENGLTHREAD_H_
#define COPENGLTHREAD_H_

enum GLOperations{
	GLOp_Resize,
	GLOp_BlitToScreen,
	GLOp_RenderModels,
	GLOp_Shutdown,
	GLOp_NoOp
};

#include <string>

class COpenGL;
class CModelList;
class CGLShaderList;
class CShader;

#include <GL/gl.h>
#include <GL/glu.h>
#include "CGLShader.h"


class COpenGLThread
{
protected:
	// ThreadFunc
	static COpenGLThread TheInstance;
	CModelList * mModels;

	// Class variables for (a single) off-screen frame buffer.
	GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;
	int mWindow_width;
	int mWindow_height;
	double scale;

	CGLShaderList * shader_list;

	bool mRun;

public:
	COpenGLThread();
	virtual ~COpenGLThread();

	void BlitToScreen();

	static void CheckOpenGLError(string function_name);

	GLuint GetFramebuffer() { return mFBO; };
	GLuint GetFramebufferTexture() { return mFBO_texture; };
	static COpenGLThread * GetInstance() { return &TheInstance; };
	GLOperations GetNext();
	CShader * GetShader(eGLShaders shader);
	int GetWindowWidth() { return mWindow_width; };
	int GetWindowHeight() { return mWindow_height; };

	void Init(int window_width, int window_height, double scale, string shader_source_dir);
	void InitFrameBuffer(void);
	void InitFrameBufferDepthBuffer(void);
	void InitFrameBufferTexture(void);

	void RegisterModels(CModelList * model_list);
	void RenderModels();
    bool Run() { return mRun; };

    void Stop();

    static int Thread();

    //    void resizeGL(int w, int h);
    //    void paintGL();
    //    void mousePressEvent(QMouseEvent *event);
    //    void mouseMoveEvent(QMouseEvent *event);
    //    void keyPressEvent(QKeyEvent *event);
};

#endif /* COPENGLTHREAD_H_ */
