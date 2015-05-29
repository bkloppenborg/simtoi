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

#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>
#include <QStandardItemModel>
#include <utility>
#include <vector>

#include "liboi.hpp"
#include "CModelList.h"
//#include "CAnimator.h"
#include "CWorkerThread.h"
#include "CTreeModel.h"

class CParameterMap;
class CParameters;

class CModelList;
typedef shared_ptr<CModelList> CModelListPtr;

class CWorkerThread;
typedef shared_ptr<CWorkerThread> CWorkerPtr;

class CGLWidget : public QGLWidget
{
    Q_OBJECT
    
protected:
    // save directory
    string mSaveDirectory;

    // Worker thread
    CWorkerPtr mWorker;

    static QGLFormat mFormat;

public:
    CGLWidget(QWidget *widget_parent);
    virtual ~CGLWidget();

    void addModel(shared_ptr<CModel> model);
	CModelPtr getModel(unsigned int model_index);
	CModelListPtr getModels() { return mWorker->GetModelList(); };
	void replaceModel(unsigned int model_index, CModelPtr new_model);
	void removeModel(unsigned int model_index);

	void addData(string filename);

	int getNData() { return mWorker->GetDataSize(); };
	int getNModels() { return mWorker->GetModelList()->size(); };

	void removeData(unsigned int data_index);

protected:
    void closeEvent(QCloseEvent *evt);

//	void paintEvent(QPaintEvent * );
	void glDraw();	// override the QGLWidget::glDraw function
	void paintGL();

public:
	void resetWidget();
protected:
    void resizeEvent(QResizeEvent *evt);
    void paintEvent(QPaintEvent * event);

public:
    void Export(QString save_folder);

public:

    QStringList GetFileFilters();
    double GetTime();
    CWorkerPtr getWorker() { return mWorker; };


    unsigned int GetImageWidth() { return mWorker->GetImageWidth(); };
    unsigned int GetImageHeight() { return mWorker->GetImageHeight(); };
    string GetSaveFolder() { return mSaveDirectory; };

public:
    void Open(string filename);

public:
    void Render();

    void Save(string filename);
    void SetScale(double scale);
    void SetFreeParameters(double * params, int n_params, bool scale_params);
    void SetSaveDirectory(string directory_path);
    void SetSize(unsigned int width, unsigned int height);
    void SetTime(double time);
    void setWavelength(double wavelength);
    void startRendering();
    void stopRendering();

private slots:

	void updateParameters();

signals:
	void modelUpdated();
//	void dataUpdated();
	void dataAdded(CDataInfo info);
	void dataRemoved(int index);
};

#endif
