#ifndef CUI_H
#define CUI_H

#include <QtGui/QMainWindow>
#include "ui_CUI.h"
#include "CGLWidget.h"

class CUI : public QMainWindow, public Ui::CUIClass
{
    Q_OBJECT

public:
    CUI();
    ~CUI();

};

#endif // CUI_H
