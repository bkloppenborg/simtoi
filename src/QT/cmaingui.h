#ifndef CMAINGUI_H
#define CMAINGUI_H

#include <QtGui/QMainWindow>
#include <string>
#include "ui_cmaingui.h"

using namespace std;

class cmaingui : public QMainWindow
{
    Q_OBJECT

private:
    Ui::cmainguiClass ui;

public:
    cmaingui(QWidget *parent = 0);
    ~cmaingui();

protected:
    void closeEvent(QCloseEvent *evt);
    string mShaderSourceDir;

private slots:
    void addGLArea();
    void delGLArea();
    void render();
};

#endif // CMAINGUI_H
