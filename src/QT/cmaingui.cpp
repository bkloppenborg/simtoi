#include "cmaingui.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>

#include "CGLWidget.h"

cmaingui::cmaingui(QWidget *parent_widget)
    : QMainWindow(parent_widget)
{
	ui.setupUi(this);

	// Set initial values:
	ui.spinModelScale->setRange(0.01, 1.0);
	ui.spinModelScale->setSingleStep(0.05);
	ui.spinModelScale->setValue(0.05);
	ui.spinModelSize->setRange(64, 1024);
	ui.spinModelSize->setSingleStep(64);
	ui.spinModelSize->setValue(128);

	// Now setup some signals and slots
	connect(ui.btnModelArea, SIGNAL(clicked(void)), this, SLOT(addGLArea(void)));
	connect(ui.btnRender, SIGNAL(clicked(void)), this, SLOT(render(void)));
}

cmaingui::~cmaingui()
{

}


void cmaingui::closeEvent(QCloseEvent *evt)
{
	QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
    for (int i = 0; i < int(windows.count()); ++i)
    {
    	CGLWidget * tmp = (CGLWidget *)windows.at(i);
    	tmp->stopRendering();
    }
    QMainWindow::closeEvent(evt);
}


void cmaingui::addGLArea()
{
	QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
    CGLWidget *widget = new CGLWidget(ui.mdiArea);
    QMdiSubWindow *sw = ui.mdiArea->addSubWindow(widget);
    sw->setWindowTitle("Model Area");
    // Init the size and scale of the model.
    sw->resize(ui.spinModelSize->value(), ui.spinModelSize->value());
    widget->SetScale(ui.spinModelScale->value());
    // Show it and start the rendering thread
    sw->show();
    widget->startRendering();
}

void cmaingui::delGLArea()
{
    CGLWidget *widget = (CGLWidget *) ui.mdiArea->activeSubWindow();
    if (widget)
    {
        widget->stopRendering();
        delete widget;
    }
}

void cmaingui::render()
{
    CGLWidget *widget = (CGLWidget *) ui.mdiArea->activeSubWindow();
    if(widget)
    {
    	widget->EnqueueOperation(GLT_Redraw);
    	//widget->EnqueueOperation(GLT_BlitToScreen);
    }
}
