/*
 * COpenGL.h
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 *
 *
 *  A singleton class.  Object responsible for all OpenGL rendering operations
 *  and storage of memory objects used by SIMTOI.
 *

 */

#ifndef COPENGL_H_
#define COPENGL_H_

#include <string>
#include <QObject>
#include <QMutexLocker>
#include <GL/gl.h>
#include <GL/glu.h>

class CModelList;
class CGLShaderList;
class CShader;
class CGLWidget;

#include "CGLShader.h"

class COpenGL : public QObject
{
	Q_OBJECT

public:
	COpenGL(CGLWidget * gl_widget);
	virtual ~COpenGL();

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

#endif /* COPENGL_H_ */
