
#ifndef GLWIDGET
#define GLWIDGET

#include <QObject>
#include <QThread>
#include <QGLWidget>
#include <QResizeEvent>
#include <QtDebug>

#include "CGLThread.h"

class CGLWidget : public QGLWidget {
    Q_OBJECT
    
protected:
    CGLThread mGLT;

public:
    CGLWidget(QWidget *parent);

    void startRendering();
    void stopRendering();
protected:
    void resizeEvent(QResizeEvent *evt);
    void paintEvent(QPaintEvent * );
    void closeEvent(QCloseEvent *evt);

};

#endif
