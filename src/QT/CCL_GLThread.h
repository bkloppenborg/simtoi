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
	CLT_Init,
	CLT_Chi,
	CLT_Chi2,
	CLT_LogLike,
	CLT_Tests
};

/// A quick class for making priority queue comparisons.  Used for CCL_GLThread, mQueue
class GLQueueComparision
{
public:
	GLQueueComparision() {};

	bool operator() (const CL_GLT_Operations & lhs, const CL_GLT_Operations & rhs) const
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
    bool mCLInitalized;
    QSemaphore mCLOpSemaphore;
    int mCLDataSet;
    double mCLValue;
    float * mCLArrayValue;
    int mCLArrayN;

    // Misc datamembers:
	bool mRun;

    int id;
    static int count;

public:
    CCL_GLThread(CGLWidget * glWidget, string shader_source_dir, string kernel_source_dir);
    ~CCL_GLThread();

    void AddModel(CModelList::ModelTypes model);

protected:
    void BlitToScreen();
    void ClearQueue();
public:
    static void CheckOpenGLError(string function_name);

    void EnqueueOperation(CL_GLT_Operations op);

	void GetChi(int data_num, float * output, int & n);
    double GetChi2(int data_num);
    double GetDataAveJD(int data_num);
    void GetFreeParameters(double * params, int n_params, bool scale_params) { mModelList->GetFreeParameters(params, n_params, scale_params); };
    int GetHeight() { return mHeight; };
	double GetLogLike(int data_num);
	CModelList * GetModelList() { return mModelList; };
    CL_GLT_Operations GetNextOperation(void);
	int GetNFreeParameters() { return mModelList->GetNFreeParameters(); };
	vector< pair<double, double> > GetFreeParamMinMaxes() { return mModelList->GetFreeParamMinMaxes(); };
	vector<string> GetFreeParamNames() { return mModelList->GetFreeParamNames(); };
	int GetNData();
	int GetNDataAllocated();
	int GetNDataAllocated(int data_num);
	int GetNDataSets();
	double GetScale() { return mScale; };
	vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderNames(void);
	int GetWidth() { return mWidth; };

    vector< pair<CModelList::ModelTypes, string> > GetModelTypes() { return mModelList->GetTypes(); };
    vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderTypes() { return mShaderList->GetTypes(); };
//    vector< pair<int, string> > GetPositionTypes() { return mGLT.GetPositionTypes(); };


protected:
    void InitFrameBuffer(void);
    void InitFrameBufferDepthBuffer(void);
    void InitFrameBufferTexture(void);

public:
    void LoadData(string filename);

    void Open(string filename);
    bool OpenCLInitialized() { return mCLInitalized; };

    static void ResetGLError();
    void resizeViewport(const QSize &size);
    void resizeViewport(int width, int height);
    void run();

    void Save(string filename);
    void SetFreeParameters(double * params, int n_params, bool scale_params);
    void SetPositionType(int model_id, CPosition::PositionTypes pos_type);
    void SetScale(double scale);
    void SetShader(int model_id, CGLShaderList::ShaderTypes shader);
    void SetTime(double t);
    void SetTimestep(double dt);
    void stop();

};
    
#endif
