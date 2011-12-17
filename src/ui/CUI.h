#ifndef CUI_H
#define CUI_H

#include <QtGui/QMainWindow>
#include "ui_CUI.h"
#include "CGLWidget.h"

class CUI : public QMainWindow
{
    Q_OBJECT

private:
    Ui::CUIClass ui;

public:
    CUI(QWidget *parent = 0);
    ~CUI();

    CGLWidget * GetGLWidget();

};

#endif // CUI_H
