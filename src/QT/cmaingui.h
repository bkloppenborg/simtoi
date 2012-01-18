#ifndef CMAINGUI_H
#define CMAINGUI_H

#include <QtGui/QMainWindow>
#include <string>
#include "ui_cmaingui.h"

using namespace std;

class cmaingui : public QMainWindow
{
    Q_OBJECT

protected:
    Ui::cmainguiClass ui;
    string mShaderSourceDir;

public:
    cmaingui(QWidget *parent = 0);
    ~cmaingui();

protected:
    void closeEvent(QCloseEvent *evt);
    void SetupComboBoxes(void);

private slots:
    void addGLArea();
    void addModel(void);
    void delGLArea();
    void render();
    void model_clicked(const QModelIndex &index);
};

#endif // CMAINGUI_H
