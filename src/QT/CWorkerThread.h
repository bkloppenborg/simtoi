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
#include <QMutex>
#include <QSemaphore>
#include <QSize>
#include <valarray>
#include <memory>
#include <queue>
#include "json/json.h"

using namespace std;

class CGLWidget;
class CTaskList;
typedef shared_ptr<CTaskList> CTaskListPtr;
class CModel;
typedef shared_ptr<CModel> CModelPtr;
class CModelList;
typedef shared_ptr<CModelList> CModelListPtr;

enum WorkerOperations
{
	ANIMATE,
	ANIMATE_STOP,
	BLIT_TO_SCREEN,
	EXPORT,
	GET_RESIDUALS,
	GET_UNCERTAINTIES,
	RENDER,
	RESIZE,
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

    // OpenCL

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
	valarray<double> * mTempArray;	// External memory. Don't allocate/deallocate.
	string mTempString;
	double mTempDouble;

public:
    CWorkerThread(CGLWidget * glWidget, QString exe_folder);
    virtual ~CWorkerThread();

public:
    void AddModel(CModelPtr model);
protected:
    void BlitToScreen();

    void CheckOpenGLError(string function_name);
protected:
    void ClearQueue();

    void Enqueue(WorkerOperations op);
public:
    void ExportResults(QString save_folder);

    CModelListPtr GetModelList() { return mModelList; };
    WorkerOperations GetNextOperation(void);
    void GetResiduals(valarray<double> & residuals);
    CTaskListPtr GetTaskList() { return mTaskList; };
    void GetUncertainties(valarray<double> & uncertainties);

    unsigned int GetImageDepth() { return mImageDepth; };
    unsigned int GetImageHeight() { return mImageHeight; };
    unsigned int GetImageWidth() { return mImageWidth; };
    double GetImageScale() { return mImageScale; };


    void Render();
protected:
    void Resize(unsigned int width, unsigned int height);
public:
    void resizeViewport(const QSize &size);
    void resizeViewport(int width, int height);
    void Restore(Json::Value input);
    void run();

    Json::Value Serialize();
    void startAnimation(double timestep);
    void stopAnimation();
    void stop();
};
    
#endif // C_WORKER_THREAD
