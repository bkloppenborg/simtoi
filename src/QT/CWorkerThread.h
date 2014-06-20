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
 * A class to interface with the workers.
 * TODO: This uses QT 3x thread structure (i.e. inherit from QThread). This should be
 *       converted to QT 4x style where we use a moveToThread command instead. See
 *       docs here: http://qt-project.org/doc/qt-4.8/QThread.html
 */
 
#ifndef C_WORKER_THREAD
#define C_WORKER_THREAD

#include <QObject>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QMutex>
#include <QSemaphore>
#include <QSize>
#include <valarray>
#include <memory>
#include <queue>
#include "json/json.h"

#include "OpenGL.h" // OpenGL includes, plus several workarounds for various OSes

// OpenGL mathematics library:
=======
#include <GL/gl.h>
#include <GL/glu.h>
#endif // __APPLE__
>>>>>>> f880df83c9585db29cd66016073ff1e8ebd44fea
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

using namespace std;

class CGLWidget;
class CTaskList;
typedef shared_ptr<CTaskList> CTaskListPtr;
class CModel;
typedef shared_ptr<CModel> CModelPtr;
class CModelList;
typedef shared_ptr<CModelList> CModelListPtr;
class COpenCL;
typedef shared_ptr<COpenCL> COpenCLPtr;

enum WorkerOperations
{
	BOOTSTRAP_NEXT,
	EXPORT,
	GET_CHI,
	GET_UNCERTAINTIES,
	OPEN_DATA,
	RENDER,
	SET_TIME,
	STOP
};

/// A quick class for making priority queue comparisons.  Used for CCL_GLThread, mQueue
class WorkerQueueComparision
{
public:
	WorkerQueueComparision() {};

	bool operator() (const WorkerOperations & lhs, const WorkerOperations & rhs) const
	{
		if(rhs == STOP)
			return true;

		return false;
	}
};

class CWorkerThread : public QThread
{
    Q_OBJECT
protected:
    // OpenGL
    CGLWidget * mGLWidget;
    unsigned int mImageDepth;
    unsigned int mImageHeight;
    double mImageScale;
    unsigned int mImageWidth;
    unsigned int mImageSamples;
    glm::mat4 mView;

    // Off-screen framebuffer (this matches the buffer created by CreateGLBuffer)
    // All rendering from the UI happens in these buffers. Results are blitted to screen.
    GLuint mFBO;
	GLuint mFBO_texture;
	GLuint mFBO_depth;
    GLuint mFBO_storage;
	GLuint mFBO_storage_texture;

    // OpenCL
    COpenCLPtr mOpenCL;

    // other data members:
    shared_ptr<CTaskList> mTaskList;
    shared_ptr<CModelList> mModelList;
    QString mExeFolder;

    // Queue:
	priority_queue<WorkerOperations, vector<WorkerOperations>, WorkerQueueComparision> mTaskQueue;
	QMutex mTaskMutex;			// For adding/removing items from the operation queue
	QSemaphore mTaskSemaphore;	// For blocking calling thread while operation finishes

	bool mRun;
	QMutex mWorkerMutex;			// Lock to have exclusive access to this object (all calls from external threads do this)
	QSemaphore mWorkerSemaphore;	// Acquire if a read/write operation is enqueued.

	// Temporary storage locations
	double * mTempArray;	// External memory. Don't allocate/deallocate.
	unsigned int mTempArraySize;
	string mTempString;
	double mTempDouble;
	unsigned int mTempUint;

public:
    CWorkerThread(CGLWidget * glWidget, QString exe_folder);
    virtual ~CWorkerThread();

public:
    void AddModel(CModelPtr model);
    void AllocateBuffer();

public:
    void BlitToBuffer(GLuint in_buffer, GLuint out_buffer);
    void BlitToScreen(GLuint FBO);
    void BootstrapNext(unsigned int maxBootstrapFailures);

    static void CheckOpenGLError(string function_name);
protected:
    void ClearQueue();
public:
    void CreateGLBuffer(GLuint & FBO, GLuint & FBO_texture, GLuint & FBO_depth, GLuint & FBO_storage, GLuint & FBO_storage_texture, int n_layers);
    void CreateGLBuffer(GLuint & FBO, GLuint & FBO_texture, GLuint & FBO_depth, GLuint & FBO_storage, GLuint & FBO_storage_texture);
    void CreateGLMultisampleRenderBuffer(unsigned int width, unsigned int height, unsigned int samples,
    		GLuint & FBO, GLuint & FBO_texture, GLuint & FBO_depth);
    void CreateGLStorageBuffer(unsigned int width, unsigned int height, unsigned int depth, GLuint & FBO_storage, GLuint & FBO_storage_texture);

    void Enqueue(WorkerOperations op);
public:
    void ExportResults(QString save_folder);

    double GetTime();
    void GetChi(double * chi, unsigned int size);
    unsigned int GetDataSize();
    QStringList GetFileFilters();
    CModelListPtr GetModelList() { return mModelList; };
    WorkerOperations GetNextOperation(void);
    CTaskListPtr GetTaskList() { return mTaskList; };
    void GetUncertainties(double * uncertainties, unsigned int size);

    unsigned int GetImageDepth() { return mImageDepth; };
    unsigned int GetImageHeight() { return mImageHeight; };
    unsigned int GetImageWidth() { return mImageWidth; };
    double GetImageScale() { return mImageScale; };
    COpenCLPtr GetOpenCL() { return mOpenCL; };
    glm::mat4 GetView() { return mView; };

    void OpenData(string filename);

    void Render();
public:
    void Restore(Json::Value input);
    void run();

    void SetScale(double scale);
    void SetSize(unsigned int width, unsigned int height);
    void SetTime(double time);
    Json::Value Serialize();
    void stop();
protected:
    void SwapBuffers();
};
    
#endif // C_WORKER_THREAD
