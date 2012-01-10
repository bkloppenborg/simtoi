#include "CUI.h"

#include <QtGui/QPushButton>
#include "COpenGL.h"

CUI::CUI()
{
	setupUi(this);

	connect(btnRender, SIGNAL(clicked(void)), wGLMain->GetGL(), SLOT(RenderModels(void)));
	connect(btnBlit, SIGNAL(clicked(void)), wGLMain->GetGL(), SLOT(BlitToScreen(void)));

}

CUI::~CUI()
{

}
