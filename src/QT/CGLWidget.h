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
#include "CMinimizerThread.h"
#include "CWorkerThread.h"
#include "CTreeModel.h"

class CModel;

class CGLWidget : public QGLWidget
{
    Q_OBJECT
    
protected:
    // save directory
    string mSaveDirectory;

    // Worker thread
    shared_ptr<CWorkerThread> mWorker;

    // Minimizer
    CMinimizerPtr mMinimizer;
    QStandardItemModel mOpenFileModel;
    CTreeModel mTreeModel;
    static QGLFormat mFormat;

public:
    CGLWidget(QWidget *widget_parent, string shader_source_dir, string cl_kernel_dir);
    virtual ~CGLWidget();

    void AddModel(shared_ptr<CModel> model);
protected:
    void closeEvent(QCloseEvent *evt);

	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:
    void Export(QString save_folder);

public:

    QStringList GetFileFilters();
    string GetMinimizerID();
    bool GetMinimizerRunning();
    double GetTime();

    QStandardItemModel * GetOpenFileModel() { return &mOpenFileModel; };
    CTreeModel * GetTreeModel() { return &mTreeModel; };
    unsigned int GetImageWidth() { return mWorker->GetImageWidth(); };
    unsigned int GetImageHeight() { return mWorker->GetImageHeight(); };
    string GetSaveFolder() { return mSaveDirectory; };

protected:
    void LoadParameters(QStandardItem * parent, CParameters * parameters);
    QList<QStandardItem *> LoadParametersHeader(QString name, CParameters * param_base);

public:
    void Open(string filename);
    void OpenData(string filename);

protected:
    void RebuildTree();
public:
    void Render();

    void Save(string filename);
    void SetScale(double scale);
    void SetFreeParameters(double * params, int n_params, bool scale_params);
    void SetMinimizer(CMinimizerPtr minimizer);
    void SetSaveDirectory(string directory_path);
    void SetSize(unsigned int width, unsigned int height);
    void SetTime(double time);
    void startMinimizer();
    void startRendering();
    void stopMinimizer();
    void stopRendering();

private slots:

	void on_mTreeModel_parameterUpdated();

	void on_minimizer_finished();

signals:
	void minimizerFinished();

};

#endif
