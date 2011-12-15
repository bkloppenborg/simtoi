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
	Resize,
	BlitToScreen,
	RenderModels,
	Shutdown,
	NoOp
};

class COpenGL;
class mModelList;

class COpenGLThread
{
protected:
	// ThreadFunc
	COpenGLThread TheInstance;

	mModelList * mModels;

	// Class variables for (a single) off-screen frame buffer.
	GLuint fbo;
	GLuint fbo_texture;
	GLuint fbo_depth;
	int window_width;
	int window_height;
	double scale;

	CGLShaderList * shader_list;

	bool mRun;

public:
	COpenGLThread();
	virtual ~COpenGLThread();

	void BlitToScreen();

	GLuint GetFramebuffer() { return fbo; };
	GLuint GetFramebufferTexture() { return fbo_texture; };
	COpenGLThread * GetInstance() { return &TheInstance; };
	CShader * GetShader(eGLShaders shader);
	int GetWindowWidth() { return window_width; };
	int GetWindowHeight() { return window_height; };

	void Init(int window_width, int window_height, double scale, string shader_source_dir);
	void InitFrameBuffer(void);
	void InitFrameBufferDepthBuffer(void);
	void InitFrameBufferTexture(void);

	void RenderModels();

    void resizeGL(int w, int h);
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

    bool Run() { return mRun; };

    void Stop();

    static int Thread();
};

#endif /* COPENGLTHREAD_H_ */
