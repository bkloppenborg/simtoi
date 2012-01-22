
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

using namespace std;

// A list of operations permitted.
enum GLT_Operations
{
	GLT_BlitToScreen,
	GLT_Resize,
	GLT_RenderModels,
	GLT_Stop
};

/// A quick class for making priority queue comparisons.  Used for CGLThread, mQueue
class GLQueueComparision
{
public:
	GLQueueComparision() {};

	bool operator() (const GLT_Operations& lhs, const GLT_Operations&rhs) const
	{
		if(rhs == GLT_Stop)
			return true;

		return false;
	}
};


class CGLThread : public QThread {
    Q_OBJECT

protected:
	priority_queue<GLT_Operations, vector<GLT_Operations>, GLQueueComparision> mQueue;
    QMutex mQueueMutex;
    QSemaphore mQueueSemaphore;
    CGLWidget * mGLWidget;
    CModelList * mModelList;
    CGLShaderList * mShaderList;
    bool mPermitResize;
    bool mResizeInProgress;
    int mWidth;
    int mHeight;
    double mScale;

    //QGLFramebufferObject * mFBO
	GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;

	bool mRun;

    bool doResize;
    float rotAngle;
    int id;
    static int count;

public:
    CGLThread(CGLWidget * glWidget, string shader_source_dir);
    ~CGLThread();

    void AddModel(eModels model);

protected:
    void BlitToScreen();
public:
    static void CheckOpenGLError(string function_name);

    void EnqueueOperation(GLT_Operations op);

    GLT_Operations GetNextOperation(void);
	vector< pair<eGLShaders, string> > GetShaderNames(void);
	CModelList * GetModelList() { return mModelList; };

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
    void SetScale(double scale);
    void SetShader(int model_id, eGLShaders shader);
    void stop();

};
    
#endif
