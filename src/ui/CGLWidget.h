/*
 * CGLWidget.h
 *
 *  Created on: Dec 15, 2011
 *      Author: bkloppenborg
 */

#ifndef CGLWIDGET_H_
#define CGLWIDGET_H_

#include <QtOpenGL/QGLWidget>

class COpenGLThread;

class CGLWidget : public QGLWidget {

    Q_OBJECT // must include this if you use Qt signals/slots

protected:
    COpenGLThread * mThread;

public:
    CGLWidget(QWidget *parent = NULL);

signals:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
//    void mousePressEvent(QMouseEvent *event);
//    void mouseMoveEvent(QMouseEvent *event);
//    void keyPressEvent(QKeyEvent *event);
};

#endif /* CGLWIDGET_H_ */
