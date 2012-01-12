#ifndef CMAINGUI_H
#define CMAINGUI_H

#include <QtGui/QMainWindow>
#include "ui_cmaingui.h"

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

private slots:
    void addGLArea();
    void delGLArea();
};

#endif // CMAINGUI_H
