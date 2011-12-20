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

#include <string>
#include <QThread>
#include <QMutexLocker>

class COpenGL;
class CModelList;
class CGLShaderList;
class CShader;
class CGLWidget;

#include <GL/gl.h>
#include <GL/glu.h>
#include "CGLShader.h"

class COpenGLThread : public QThread
{
	Q_OBJECT

public:
	COpenGLThread(CGLWidget * gl_widget);
	virtual ~COpenGLThread();

protected:
	QMutex mGLMutex;
	CModelList * mModels;

	CGLWidget * mGLWidget;

	// Class variables for (a single) off-screen frame buffer.
	GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;
	int mWindow_width;
	int mWindow_height;
	double mScale;

	CGLShaderList * shader_list;

public slots:
	void InitOpenGL();
	void Resize(int w, int h);
	void Redraw() { BlitToScreen(); };
	void BlitToScreen();
	void RenderModels();
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void keyPressEvent(QKeyEvent *event);

signals:
	void RenderComplete();

public:
	static void CheckOpenGLError(string function_name);

	GLuint GetFramebuffer() { return mFBO; };
	GLuint GetFramebufferTexture() { return mFBO_texture; };
	CShader * GetShader(eGLShaders shader);
	int GetWindowWidth() { return mWindow_width; };
	int GetWindowHeight() { return mWindow_height; };

	void Init(int window_width, int window_height, double scale, string shader_source_dir);
	void InitFrameBuffer(void);
	void InitFrameBufferDepthBuffer(void);
	void InitFrameBufferTexture(void);

	void RegisterModels(CModelList * model_list);
	void run();
};

#endif /* COPENGLTHREAD_H_ */
