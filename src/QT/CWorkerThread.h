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
#include <valarray>
#include <memory>
#include <queue>

using namespace std;

class CGLWidget;
class CWorkerList;

enum WorkerOperations
{
	GET_RESIDUALS,
	GET_UNCERTAINTIES,
	RENDER,
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

protected:
    QString mExeFolder;

    // Queue:
	priority_queue<WorkerOperations, vector<WorkerOperations>, WorkerQueueComparision> mOpQueue;
	QMutex mOpQueueMutex;
	QSemaphore mOpQueueSemaphore;

	bool mRun;
	QMutex mMutex;
	QSemaphore mSemaphore;

	// Temporary storage locations
	valarray<double> * mTempArray;

public:
    CWorkerThread(CGLWidget * glWidget, QString exe_folder);
    virtual ~CWorkerThread();

    void Enqueue(WorkerOperations op);

    WorkerOperations GetNextOperation(void);
    void GetResiduals(valarray<double> & residuals);
    void GetUncertainties(valarray<double> & uncertainties);

    void run();
};
    
#endif // C_WORKER_THREAD
