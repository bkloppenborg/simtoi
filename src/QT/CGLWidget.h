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

#include "CCL_GLThread.h"
#include "CMinimizerThread.h"
#include "liboi.hpp"
#include "CModelList.h"
#include "CMinimizer.h"

class CModel;
class CTreeModel;

class CGLWidget : public QGLWidget
{
    Q_OBJECT
    
protected:
    CCL_GLThread mGLT;
    CMinimizerThread mMinThread;
    QStandardItemModel * mOpenFileModel;
    CTreeModel * mTreeModel;
    static QGLFormat mFormat;

public:
    CGLWidget(QWidget *widget_parent, string shader_source_dir, string cl_kernel_dir);
    virtual ~CGLWidget();

    void AddModel(CModelList::ModelTypes model);
protected:
    void closeEvent(QCloseEvent *evt);
public:

    void EnqueueOperation(CL_GLT_Operations op);
	vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderNames(void) { return mGLT.GetShaderNames(); };

protected:
	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:

    vector< pair<CModelList::ModelTypes, string> > GetModelTypes() { return mGLT.GetModelTypes(); };
    vector< pair<CGLShaderList::ShaderTypes, string> > GetShaderTypes() { return mGLT.GetShaderTypes(); };
//    vector< pair<int, string> > GetPositionTypes() { return mGLT.GetPositionTypes(); };

    double GetFlux() { return mGLT.GetFlux(); };
    void GetImage(float * image, unsigned int width, unsigned int height, unsigned int depth) { mGLT.GetImage(image, width, height, depth); };
    unsigned int GetImageDepth() { return mGLT.GetImageDepth(); };
    unsigned int GetImageHeight() { return mGLT.GetImageHeight(); };
    unsigned int GetImageWidth() { return mGLT.GetImageWidth(); };
    float GetImageScale() { return mGLT.GetScale(); };
    int GetNData() { return mGLT.GetNData(); };
    int GetNDataSets() { return mGLT.GetNDataSets(); };
    double GetDataAveJD(int data_num) { return mGLT.GetDataAveJD(data_num); };
    int GetNModels() { return mGLT.GetModelList()->size(); };
    CModelList * GetModelList() { return mGLT.GetModelList(); };
    QStandardItemModel * GetOpenFileModel() { return mOpenFileModel; };
    string GetSaveFileBasename();
    double GetScale() { return mGLT.GetScale(); };
    CTreeModel * GetTreeModel() { return mTreeModel; };


    void LoadData(string filename) { mGLT.LoadData(filename); };
    void LoadMinimizer(CMinimizer::MinimizerTypes minimizer_type);
protected:
    void LoadParameters(QStandardItem * parent, CParameters * parameters);
    QList<QStandardItem *> LoadParametersHeader(QString name, CParameters * param_base);

public:
    void Open(string filename);
    bool OpenCLInitialized() { return mGLT.OpenCLInitialized(); };

    void RunMinimizer();
protected:
    void RebuildTree();
public:
    void RemoveData(int data_num) { mGLT.RemoveData(data_num); };

public:
    void Save(string location) { mGLT.Save(location); };
    void SaveImage(string filename) { mGLT.SaveImage(filename); };
    void SetFreeParameters(double * params, int n_params, bool scale_params);
    void SetScale(double scale);
    void SetShader(int model_id, CGLShaderList::ShaderTypes shader);
    void SetPositionType(int model_id, CPosition::PositionTypes pos_type);
    void SetSaveFileBasename(string filename);
    void SetTime(double t) { mGLT.SetTime(t); };
    void SetTimestep(double dt) { mGLT.SetTimestep(dt); };
    void StopMinimizer();

    void startRendering();
    void stopRendering();

signals:
	void MinimizationFinished(QWidget * parent_window);

public slots:
	void MinimizerExit();

};

#endif
