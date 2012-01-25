/*
 * CGLShader.h
 *
 *  Created on: Jan 24, 2011
 *      Author: bkloppenborg
 *
 * A common class to encapsulate access to OpenCL and OpenGL.
 * This class manages all CL/GL memory allocation/deallocation, rendering, and computations.
 *
 */


#ifndef GLTHREAD
#define GLTHREAD

#include <QObject>
#include <QThread>
#include <QSize>
#include <QMutex>
#include <QSemaphore>
#include <string>
#include <queue>

#include <GL/gl.h>
#include <GL/glu.h>
#include "CModel.h"
#include "CModelList.h"
#include "CGLShaderList.h"

class CGLWidget;
class CModel;
class CGLShaderWrapper;
class CLibOI;

using namespace std;

// A list of operations permitted.
enum CL_GLT_Operations
{
	GLT_BlitToScreen,
	GLT_Resize,
	GLT_RenderModels,
	GLT_Animate,
	GLT_StopAnimate,
	GLT_Stop,
	CLT_Init
};

/// A quick class for making priority queue comparisons.  Used for CCL_GLThread, mQueue
class GLQueueComparision
{
public:
	GLQueueComparision() {};

	bool operator() (const CL_GLT_Operations& lhs, const CL_GLT_Operations&rhs) const
	{
		if(rhs == GLT_Stop)
			return true;

		return false;
	}
};


class CCL_GLThread : public QThread {
    Q_OBJECT

protected:

    // Window-related items:
    bool mPermitResize;
    bool mResizeInProgress;
    int mWidth;
    int mHeight;
    double mScale;
    double mDepth;

    // Queue:
	priority_queue<CL_GLT_Operations, vector<CL_GLT_Operations>, GLQueueComparision> mQueue;
    QMutex mQueueMutex;
    QSemaphore mQueueSemaphore;

    // OpenGL / rendering / FBOs
    CGLWidget * mGLWidget;
    CModelList * mModelList;
    CGLShaderList * mShaderList;
    GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;

    // OpenCL:
    CLibOI * mCL;
    string mKernelSourceDir;

    // Misc datamembers:
	bool mRun;

    bool doResize;
    float rotAngle;
    int id;
    static int count;

public:
    CCL_GLThread(CGLWidget * glWidget, string shader_source_dir, string kernel_source_dir);
    ~CCL_GLThread();

    void AddModel(eModels model);

protected:
    void BlitToScreen();
    void ClearQueue();
public:
    static void CheckOpenGLError(string function_name);

    void EnqueueOperation(CL_GLT_Operations op);

	int GetHeight() { return mHeight; };
    CL_GLT_Operations GetNextOperation(void);
	vector< pair<eGLShaders, string> > GetShaderNames(void);
	CModelList * GetModelList() { return mModelList; };
	float GetScale() { return mScale; };
	int GetWidth() { return mWidth; };

protected:
    void InitFrameBuffer(void);
    void InitFrameBufferDepthBuffer(void);
    void InitFrameBufferTexture(void);

private:
    void glDrawTriangle();

public:
    static void ResetGLError();
    void resizeViewport(const QSize &size);
    void resizeViewport(int width, int height);
    void run();

    void SetParameters(float * params, int n_params);
    void SetPositionType(int model_id, ePositionTypes pos_type);
    void SetScale(double scale);
    void SetShader(int model_id, eGLShaders shader);
    void SetTime(double t);
    void SetTimestep(double dt);
    void stop();

};
    
#endif
