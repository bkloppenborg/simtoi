/*
 * CGLShader.h
 *
 *  Created on: Jan 24, 2011
 *      Author: bkloppenborg
 */
 
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
 
 /*
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
#include "CModelList.h"
#include "CGLShaderList.h"
#include "liboi.hpp"

class CGLWidget;
class CModel;
class CGLShaderWrapper;

using namespace std;
using namespace liboi;

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
	CLT_Flux,
	CLT_Chi,
	CLT_Chi2,
	CLT_LogLike,
	CLT_Tests,
	CLT_CopyImage,
	CLT_SaveImage,
	CLT_GetData,
	CLT_GetChi2_Elements
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
    // NOTE: these must be ints, not unsigned ints, for OpenGL.
    int mImageWidth;
    int mImageHeight;
    int mImageDepth;
    double mAreaDepth;
    double mScale;

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
    GLuint mFBO_storage;
	GLuint mFBO_storage_texture;

	GLsizei mSamples;

    // OpenCL:
    CLibOI * mCL;
    string mKernelSourceDir;
    bool mCLInitalized;
    QSemaphore mCLOpSemaphore;
    // TODO: We should serialize access to these variables:
    int mCLDataSet;
    double mCLValue;
    float * mCLArrayValue;
    unsigned int mCLArrayN;
    string mCLString;

    // Misc datamembers:
	bool mRun;
	bool mIsRunning;

    int id;
    static int count;

public:
    CCL_GLThread(CGLWidget * glWidget, string shader_source_dir, string kernel_source_dir);
    virtual ~CCL_GLThread();

    void AddModel(CModelList::ModelTypes model);

    void BlitToBuffer(GLuint in_buffer, GLuint out_buffer, unsigned int out_layer);
protected:
    void BlitToScreen();

public:
    static void CheckOpenGLError(string function_name);
protected:
    void ClearQueue();

public:
    void 	EnqueueOperation(CL_GLT_Operations op);
    void 	ExportResults(string base_filename);

	void 	GetChi(int data_num, float * output, int & n);
    double GetChi2(int data_num);
    OIDataList GetData(unsigned int data_num);
    double GetDataAveJD(int data_num);
    unsigned int GetImageDepth() { return mImageDepth; };
    double GetFlux();
    void 	GetFreeParameters(double * params, int n_params, bool scale_params) { mModelList->GetFreeParameters(params, n_params, scale_params); };
    unsigned int GetImageHeight() { return mImageHeight; };
    void 	GetImage(float * image, unsigned int width, unsigned int height, unsigned int depth);
	double GetLogLike(int data_num);
	CModelList * GetModelList() { return mModelList; };
    CL_GLT_Operations GetNextOperation(void);
	int 	GetNFreeParameters() { return mModelList->GetNFreeParameters(); };
	vector< pair<double, double> > GetFreeParamMinMaxes() { return mModelList->GetFreeParamMinMaxes(); };
	double GetFreeParameterPriorProduct() { return mModelList->GetFreeParameterPriorProduct(); };
	vector<string> GetFreeParamNames() { return mModelList->GetFreeParamNames(); };
	int 	GetNData();
	int 	GetNDataAllocated();
	int 	GetNDataAllocated(int data_num);
	int 	GetNDataSets();
	int		GetNT3(int data_num);
	int		GetNV2(int data_num);
	double GetScale() { return mScale; };
	vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderNames(void);
	unsigned int GetImageWidth() { return mImageWidth; };

	bool 	IsRunning() { return mIsRunning; };

    vector< pair<CModelList::ModelTypes, string> > GetModelTypes() { return mModelList->GetTypes(); };
    vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderTypes() { return mShaderList->GetTypes(); };
//    vector< pair<int, string> > GetPositionTypes() { return mGLT.GetPositionTypes(); };


protected:
    void 	InitFrameBuffers(void);
    void 	InitMultisampleRenderBuffer(void);
    void 	InitStorageBuffer(void);

public:
    int LoadData(string filename);
    int LoadData(const OIDataList & data);

    void Open(string filename);
    bool OpenCLInitialized() { return mCLInitalized; };

    void RemoveData(int data_num);
    static void ResetGLError();
	void ReplaceData(unsigned int old_data_id, const OIDataList & new_data);
    void resizeViewport(const QSize &size);
    void resizeViewport(int width, int height);
    void run();

    void Save(string filename);
    void SaveImage(string filename);
    void SetFreeParameters(double * params, unsigned int n_params, bool scale_params);
    void SetPositionType(int model_id, CPosition::PositionTypes pos_type);
    void SetScale(double scale);
    void SetShader(int model_id, CGLShaderList::ShaderTypes shader);
    void SetTime(double t);
    void SetTimestep(double dt);
    void start();
    void stop();

};
    
#endif
