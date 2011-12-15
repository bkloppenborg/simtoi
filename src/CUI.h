#ifndef CUI_H
#define CUI_H

#include <QtGui/QMainWindow>
#include "ui_CUI.h"

class CUI : public QMainWindow
{
    Q_OBJECT

public:
    CUI(QWidget *parent = 0);
    ~CUI();

private:
    Ui::CUIClass ui;
};

#endif // CUI_H
