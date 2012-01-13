#include "cmaingui.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVariant>
#include <QString>
#include <vector>
#include <utility>

#include "CGLWidget.h"
#include "enumerations.h"
#include "CGLShaderList.h"
#include "CModelList.h"

cmaingui::cmaingui(QWidget *parent_widget)
    : QMainWindow(parent_widget)
{
	ui.setupUi(this);

	// Set initial values for the spinboxes:
	ui.spinModelScale->setRange(0.01, 1.0);
	ui.spinModelScale->setSingleStep(0.05);
	ui.spinModelScale->setValue(0.05);
	ui.spinModelSize->setRange(64, 1024);
	ui.spinModelSize->setSingleStep(64);
	ui.spinModelSize->setValue(128);

	// Set up the combo boxes:
	SetupComboBoxes();

	// Now setup some signals and slots
	connect(ui.btnModelArea, SIGNAL(clicked(void)), this, SLOT(addGLArea(void)));

	// TODO: Remove this, shouldn't be hard-coded!
	mShaderSourceDir = "/home/bkloppenborg/workspace/simtoi/src/shaders/";
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
    CGLWidget *widget = new CGLWidget(ui.mdiArea, mShaderSourceDir);
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
    	widget->EnqueueOperation(GLT_RenderModels);
    }
}

void cmaingui::SetupComboBoxes()
{
	const char * tmp_string;
	// First get a list of the available models and enumerated types.  Append those to the combo box
	CModelList tmp1 = CModelList();
	vector< pair<eModels, string> > models = tmp1.GetList_AllModels();
	eModels tmp_model_id;

	for(vector<pair<eModels, string> >::iterator it = models.begin(); it != models.end(); ++it)
	{
		tmp_string = (*it).second.c_str();
		tmp_model_id = (*it).first;
		ui.cboModels->addItem(QString(tmp_string), QVariant(tmp_model_id));
	}

	// Now do the same for the shaders:
	CGLShaderList tmp2 = CGLShaderList(mShaderSourceDir);
	vector< pair<eGLShaders, string> > shaders = tmp2.GetShaderNames();
	eGLShaders tmp_shader_id;

	for(vector<pair<eGLShaders, string> >::iterator it = shaders.begin(); it != shaders.end(); ++it)
	{
		tmp_string = (*it).second.c_str();
		tmp_shader_id = (*it).first;
		ui.cboShaders->addItem(QString(tmp_string), QVariant(tmp_shader_id));
	}

}
