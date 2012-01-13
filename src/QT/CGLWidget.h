
#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>

#include "CGLThread.h"

class CModel;

class CGLWidget : public QGLWidget {
    Q_OBJECT
    
protected:
    CGLThread mGLT;

public:
    CGLWidget(QWidget *parent, string shader_source_dir);

    void AppendModel(CModel * model);

    void EnqueueOperation(GLT_Operations op);

protected:
    void closeEvent(QCloseEvent *evt);

	void paintEvent(QPaintEvent * );

    void resizeEvent(QResizeEvent *evt);

public:
    void SetScale(double scale);
    void startRendering();
    void stopRendering();
};

#endif
