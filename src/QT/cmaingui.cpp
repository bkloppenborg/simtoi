#include "cmaingui.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVariant>
#include <QString>
#include <QMessageBox>
#include <QTreeView>
#include <QStringList>
#include <QFileDialog>
#include <vector>
#include <utility>

#include "CGLWidget.h"
#include "enumerations.h"
#include "CGLShaderList.h"
#include "CTreeModel.h"


Q_DECLARE_METATYPE(eModels);
Q_DECLARE_METATYPE(eGLShaders);

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
	ui.spinTimestep->setValue(0.10);
	ui.spinTimestep->setSingleStep(0.1);
	ui.spinTimestep->setRange(0, 10000);

	mAnimating = false;

	// Now setup some signals and slots
	connect(ui.btnModelArea, SIGNAL(clicked(void)), this, SLOT(addGLArea(void)));
	connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subwindowSelected(QMdiSubWindow*)));
	connect(ui.btnStartStop, SIGNAL(clicked(void)), this, SLOT(Animation_StartStop(void)));
	connect(ui.btnReset, SIGNAL(clicked(void)), this, SLOT(Animation_Reset(void)));
	connect(ui.btnMinimizer, SIGNAL(clicked(void)), this, SLOT(RunMinimizer(void)));
	connect(ui.btnLoadData, SIGNAL(clicked(void)), this, SLOT(LoadData(void)));

	// Get the application path,
	string app_path = QCoreApplication::applicationDirPath().toStdString();
	mShaderSourceDir = app_path + "/shaders/";
	mKernelSourceDir = app_path + "/kernels/";
	mDataDir = "./";
}

cmaingui::~cmaingui()
{

}

void cmaingui::Animation_StartStop()
{
	CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
	if(mAnimating)
	{
		widget->EnqueueOperation(GLT_StopAnimate);
		mAnimating = false;
		ui.btnStartStop->setText("Start");
	}
	else
	{
		widget->SetTimestep(ui.spinTimestep->value());
		widget->EnqueueOperation(GLT_Animate);
		mAnimating = true;
		ui.btnStartStop->setText("Stop");
	}
}

void cmaingui::Animation_Reset()
{
	CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
	widget->SetTime(0);
	widget->EnqueueOperation(GLT_RenderModels);

}


void cmaingui::closeEvent(QCloseEvent *evt)
{
	QList<QMdiSubWindow *> windows = ui.mdiArea->subWindowList();
    for (int i = int(windows.count()) - 1; i > 0; i--)
    {
    	CGLWidget * tmp = (CGLWidget *)windows.at(i)->widget();
    	tmp->stopRendering();
    }
    QMainWindow::closeEvent(evt);
}


void cmaingui::addGLArea()
{
	// Create a new subwindow with a title and close button:
    CGLWidget *widget = new CGLWidget(ui.mdiArea, mShaderSourceDir, mKernelSourceDir);
    QMdiSubWindow *sw = ui.mdiArea->addSubWindow(widget, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint);
    sw->setWindowTitle("Model Area");

    // TODO: This is approximately right for my machine, probably not ok on other OSes.
    int frame_width = 8;
    int frame_height = 28;
    int model_width = ui.spinModelSize->value();
    int model_height = ui.spinModelSize->value();
    sw->setFixedSize(model_width + frame_width, model_height + frame_height);
    //sw->resize(model_width + frame_width, model_height + frame_height);
    sw->show();

    widget->resize(model_width, model_height);
    widget->SetScale(ui.spinModelScale->value());
    widget->startRendering();

    // TODO: Remove later.  Add a sphere:
    widget->AddModel(MDL_SPHERE);
    widget->SetShader(0, SHDR_LD_HESTEROFFER1997);
//    widget->AddModel(MDL_CYLINDER);
//    widget->SetPositionType(1, POSITION_ORBIT);

	// Set the model
	ui.treeModels->setHeaderHidden(false);
	ui.treeModels->resizeColumnToContents(1);

	// Now connect the slot
	connect(widget->GetTreeModel(), SIGNAL(parameterUpdated(void)), this, SLOT(render(void)));

	// If the load data button isn't enabled, turn it on
	ButtonCheck();
}

void cmaingui::addModel(void)
{
//	QMdiSubWindow * window = ui.mdiArea->activeSubWindow();
//
//	if(window != NULL)
//	{
//		CGLWidget * widget = (CGLWidget*) window->widget();
//		QVariant tmp = ui.cboModels->itemData(ui.cboModels->currentIndex());
//		eModels model = tmp.value<eModels>();
////		tmp = ui.cboShaders->itemData(ui.cboShaders->currentIndex());
////		eGLShaders shader = tmp.value<eGLShaders>();
//
//		widget->AddModel(model);
//	}
//	else
//	{
//		QMessageBox msgBox;
//		msgBox.setText("Please select a window to which the model may be added.");
//		msgBox.exec();
//	}
}

/// Checks to see which buttons can be enabled/disabled.
void cmaingui::ButtonCheck()
{
	ui.btnAddModel->setEnabled(false);
	ui.btnLoadData->setEnabled(false);
	ui.btnRemoveData->setEnabled(false);
	ui.btnDeleteModel->setEnabled(false);
	ui.btnStartStop->setEnabled(false);
	ui.btnReset->setEnabled(false);
	ui.btnMinimizer->setEnabled(false);

    QMdiSubWindow * sw = ui.mdiArea->activeSubWindow();
    if(!sw)
    	return;

	ui.btnAddModel->setEnabled(true);
	ui.btnLoadData->setEnabled(true);
	ui.btnStartStop->setEnabled(true);
	ui.btnReset->setEnabled(true);
	ui.btnMinimizer->setEnabled(true);

	CGLWidget * widget = (CGLWidget*) sw->widget();
	if(widget->GetOpenFileModel()->rowCount() > 0)
		ui.btnRemoveData->setEnabled(true);

	if(widget->GetTreeModel()->rowCount() > 0)
		ui.btnDeleteModel->setEnabled(true);
}

void cmaingui::delGLArea()
{
    CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
    if (widget)
    {
        widget->stopRendering();
        delete widget;
    }

    ButtonCheck();
}

void cmaingui::RunMinimizer()
{
    CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();

    if(!widget->OpenCLInitialized())
    	widget->EnqueueOperation(CLT_Init);

    widget->LoadMinimizer();
    widget->RunMinimizer();
}

/// Loads OIFITS data into the current selected subwindow
void cmaingui::LoadData()
{
	string tmp;
	int dir_size = 0;

	// Ensure there is a selected widget, if not immediately return.
    QMdiSubWindow * sw = ui.mdiArea->activeSubWindow();
    if(!sw)
    {
		QMessageBox msgBox;
		msgBox.setText("You must have an active model region before you can load data.");
		msgBox.exec();
    	return;
    }

    // Get access to the current widget and QStandardItemModel:
    CGLWidget *widget = (CGLWidget*) sw->widget();
    QStandardItemModel * model = widget->GetOpenFileModel();

    // Open a dialog, get a list of file that the user selected:
    QFileDialog dialog(this);
    dialog.setDirectory(QString::fromStdString(mDataDir));
    dialog.setNameFilter(tr("Data Files (*.fit *.fits *.oifits)"));
    dialog.setFileMode(QFileDialog::ExistingFiles);

    QStringList filenames;
    QString dir = "";
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
	}

	// Now pull out the data directory (to make file display cleaner)
	if(filenames.size() > 0)
		mDataDir = QFileInfo(filenames[0]).absolutePath().toStdString();

	for(int i = 0; i < filenames.size(); i++)
	{
		// Tell the widget to load the data file and append a row to its file list:
		tmp = filenames[i].toStdString();
		dir_size = tmp.size() - mDataDir.size();
		widget->LoadData(tmp);
		model->appendRow(new QStandardItem(QString::fromStdString( tmp.substr(mDataDir.size() + 1, dir_size) )));
	}

	ButtonCheck();
}

/// Removes the current selected data set.
void cmaingui::RemoveData()
{
//	// Ensure there is a selected widget, if not immediately return.
//    QMdiSubWindow * sw = ui.mdiArea->activeSubWindow();
//    if(!sw)
//    	return;
//
//    // Get access to the current widget, and QStandardItemModel list
//    CGLWidget *widget = (CGLWidget*) sw->widget();
//    QStandardItemModel * list = widget->GetOpenFileModel();
//
//    // Get the selected indicies:
////    QModelIndexList selection = ui.listOpenFiles->selectedIndexes();



}

void cmaingui::render()
{
    CGLWidget * widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
    if(widget)
    {
    	widget->EnqueueOperation(GLT_RenderModels);
    }
}

void cmaingui::subwindowSelected(QMdiSubWindow * mdi_subwindow)
{
	ButtonCheck();

    QMdiSubWindow * sw = ui.mdiArea->activeSubWindow();
    if(!sw)
    	return;

    CGLWidget *widget = (CGLWidget*) sw->widget();
    ui.listOpenFiles->setModel(widget->GetOpenFileModel());
	ui.treeModels->setModel(widget->GetTreeModel());
}
