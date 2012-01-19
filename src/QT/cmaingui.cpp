#include "cmaingui.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVariant>
#include <QString>
#include <QMessageBox>
#include <QTreeView>
#include <QStringList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <vector>
#include <utility>

#include "CGLWidget.h"
#include "enumerations.h"
#include "CGLShaderList.h"
#include "CModelList.h"

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

	// Set up the combo boxes:
	SetupComboBoxes();

	// Now setup some signals and slots
	connect(ui.btnModelArea, SIGNAL(clicked(void)), this, SLOT(addGLArea(void)));
	connect(ui.treeModels, SIGNAL(clicked(QModelIndex)), this, SLOT(model_clicked(QModelIndex)));

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
	// Create a new subwindow with a title and close button:
    CGLWidget *widget = new CGLWidget(ui.mdiArea, mShaderSourceDir);
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
    widget->AddModel(MDL_CYLINDER);
    float * tmp = new float[4];
    tmp[0] = 2.0;
    tmp[1] = 0.9;
    tmp[2] = 3.0;
    tmp[3] = 0.5;
    widget->SetParameters(tmp, 4);



    // Just messing around...

//	QStandardItemModel * model = new QStandardItemModel();
//	QStringList labels = QStringList();
//	labels << "Name" << "Enabled" << "Value";
//	//model->setHorizontalHeaderLabels(labels);
//	for(int r=0; r<5; r++)
//	{
//		QList<QStandardItem *>  items;
//		QStandardItem *item;
//
//		// Add icon to Column1
//		item = new QStandardItem(QString("Row %0").arg(r));
//		items << item;
//
//		// Next item
//		item = new QStandardItem;
//		item->setEditable(true);
//		item->setData(QVariant((bool)true), Qt::DisplayRole);
//		items << item;
//
//		// Next item
//		item = new QStandardItem;
//		item->setEditable(true);
//		item->setData(QVariant((int)1), Qt::DisplayRole);
//		items << item;
//
//		// Next item
//		item = new QStandardItem;
//		item->setEditable( true );
//		item->setData(QVariant((double)2.34), Qt::DisplayRole);
//		items << item;
//
//		// add row in columns
//		model->appendRow( items );
//	}
//
//	ui.treeModels->setModel(model);
//	ui.treeModels->setHeaderHidden(true);

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
//	const char * tmp_string;
//	// First get a list of the available models and enumerated types.  Append those to the combo box
//	CModelList tmp1 = CModelList();
//	vector< pair<eModels, string> > models = tmp1.GetList_AllModels();
//	eModels tmp_model_id;
//
//	for(vector<pair<eModels, string> >::iterator it = models.begin(); it != models.end(); ++it)
//	{
//		tmp_string = (*it).second.c_str();
//		tmp_model_id = (*it).first;
//		ui.cboModels->addItem(QString(tmp_string), QVariant(tmp_model_id));
//	}

//	// Now do the same for the shaders:
//	CGLShaderList tmp2 = CGLShaderList(mShaderSourceDir);
//	vector< pair<eGLShaders, string> > shaders = tmp2.GetShaderNames();
//	eGLShaders tmp_shader_id;
//
//	for(vector<pair<eGLShaders, string> >::iterator it = shaders.begin(); it != shaders.end(); ++it)
//	{
//		tmp_string = (*it).second.c_str();
//		tmp_shader_id = (*it).first;
//		ui.cboShaders->addItem(QString(tmp_string), QVariant(tmp_shader_id));
//	}

}

void cmaingui::model_clicked(const QModelIndex & index)
{
	QString text = ui.treeModels->currentIndex().data().toString();
	printf("Label: %s \n", text.toStdString().c_str());
}
