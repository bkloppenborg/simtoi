#ifndef CMAINGUI_H
#define CMAINGUI_H

#include <QtGui/QMainWindow>
#include <string>
#include <QStandardItem>
#include <QStandardItemModel>

#include "ui_cmaingui.h"
class CParameters;

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
    void LoadParameters(QStandardItem * parent, CParameters * parameters);
    QList<QStandardItem *> LoadParametersHeader(QString name, CParameters * param_base);

private slots:
    void addGLArea();
    void addModel(void);
    void delGLArea();
    void render();
    void model_clicked(const QModelIndex &index);
};

#endif // CMAINGUI_H
