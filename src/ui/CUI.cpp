#include "CUI.h"

CUI::CUI(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
}

CUI::~CUI()
{

}

CGLWidget * CUI::GetGLWidget()
{
	return ui.wGLMain;
}
