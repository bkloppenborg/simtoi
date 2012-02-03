#ifndef CMAINGUI_H
#define CMAINGUI_H

#include <QtGui/QMainWindow>
#include <string>
#include <QStandardItem>
#include <QStandardItemModel>
#include "ui_cmaingui.h"

class CParameters;
class CParameterItem;

using namespace std;

class cmaingui : public QMainWindow
{
    Q_OBJECT

protected:
    Ui::cmainguiClass ui;
    string mShaderSourceDir;
    string mKernelSourceDir;
    bool mAnimating;

    string mDataDir;	// Stores the previously opened directory in the file dialog

public:
    cmaingui(QWidget *parent = 0);
    ~cmaingui();

protected:
    void closeEvent(QCloseEvent *evt);

private slots:
    void addGLArea();
    void addModel(void);
    void delGLArea();
    void render();
    void Animation_StartStop();
    void Animation_Reset();
    void RunMinimizer();
    void LoadData();
    void RemoveData();

public slots:
	void subwindowSelected(QMdiSubWindow * window);
};

#endif // CMAINGUI_H
