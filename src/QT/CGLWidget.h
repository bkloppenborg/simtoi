
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
#include "CLibOI.h"
#include "CModelList.h"

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

public:
    CGLWidget(QWidget *widget_parent, string shader_source_dir, string cl_kernel_dir);
    ~CGLWidget();

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

    int GetNModels() { return mGLT.GetModelList()->size(); };
    CModelList * GetModelList() { return mGLT.GetModelList(); };
    QStandardItemModel * GetOpenFileModel() { return mOpenFileModel; };
    CTreeModel * GetTreeModel() { return mTreeModel; };

    void LoadData(string filename) { mGLT.LoadData(filename); };
    void LoadMinimizer();
protected:
    void LoadParameters(QStandardItem * parent, CParameters * parameters);
    QList<QStandardItem *> LoadParametersHeader(QString name, CParameters * param_base);
public:
    bool OpenCLInitialized() { return mGLT.OpenCLInitialized(); };

    void RunMinimizer();
protected:
    void RebuildTree();

public:
    void SetFreeParameters(float * params, int n_params);
    void SetScale(double scale);
    void SetShader(int model_id, CGLShaderList::ShaderTypes shader);
    void SetPositionType(int model_id, CPosition::PositionTypes pos_type);
    void SetTime(double t) { mGLT.SetTime(t); };
    void SetTimestep(double dt) { mGLT.SetTimestep(dt); };

    void startRendering();
    void stopRendering();
};

#endif
