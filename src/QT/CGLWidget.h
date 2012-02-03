
#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>
#include <QStandardItemModel>

#include "CCL_GLThread.h"
#include "CMinimizerThread.h"
#include "CLibOI.h"

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
    CGLWidget(QWidget *parent, string shader_source_dir, string cl_kernel_dir);
    ~CGLWidget();

    void AddModel(eModels model);
protected:
    void closeEvent(QCloseEvent *evt);
public:

    void EnqueueOperation(CL_GLT_Operations op);
	vector< pair<eGLShaders, string> > GetShaderNames(void) { return mGLT.GetShaderNames(); };

protected:
	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:
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

    void SetParameters(float * params, int n_params);
    void SetScale(double scale);
    void SetShader(int model_id, eGLShaders shader);
    void SetPositionType(int model_id, ePositionTypes pos_type);
    void SetTime(double t) { mGLT.SetTime(t); };
    void SetTimestep(double dt) { mGLT.SetTimestep(dt); };

    void startRendering();
    void stopRendering();
};

#endif
