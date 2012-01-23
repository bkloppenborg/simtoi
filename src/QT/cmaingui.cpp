#include "cmaingui.h"

#include <QWidgetList>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QVariant>
#include <QString>
#include <QMessageBox>
#include <QTreeView>
#include <QStringList>
#include <vector>
#include <utility>

#include "CGLWidget.h"
#include "enumerations.h"
#include "CGLShaderList.h"
#include "CModel.h"
#include "CModelList.h"
#include "CTreeModel.h"
#include "CParameterItem.h"

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

	// Now setup some signals and slots
	connect(ui.btnModelArea, SIGNAL(clicked(void)), this, SLOT(addGLArea(void)));
	connect(ui.mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(subwindowSelected(QMdiSubWindow*)));
	connect(ui.btnAnimate, SIGNAL(clicked(void)), this, SLOT(Animate(void)));

	// TODO: Remove this, shouldn't be hard-coded!
	mShaderSourceDir = "/home/bkloppenborg/workspace/simtoi/src/shaders/";
}

cmaingui::~cmaingui()
{
	mAnimating = false;
}

void cmaingui::Animate()
{
	CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
	if(mAnimating)
	{
		widget->EnqueueOperation(GLT_StopAnimate);
		widget->SetTimestep(0);
		widget->SetTime(0);
		mAnimating = false;
		ui.btnAnimate->setText("Animate");
	}
	else
	{
		widget->SetTimestep(ui.spinTimestep->value());
		widget->EnqueueOperation(GLT_Animate);
		mAnimating = true;
		ui.btnAnimate->setText("Stop Animation");
	}
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
    widget->SetPositionType(1, POSITION_ORBIT);
    float * tmp = new float[4];
    tmp[0] = 2.0;
    tmp[1] = 0.9;
    tmp[2] = 3.0;
    tmp[3] = 0.5;
    widget->SetParameters(tmp, 4);

    // Just messing around...
    CTreeModel * TreeModel = new CTreeModel();
	QStringList labels = QStringList();
	labels << "Name" << "Free" << "Value";
	TreeModel->setColumnCount(3);
	TreeModel->setHorizontalHeaderLabels(labels);
	CModelList * model_list = widget->GetModelList();

	QList<QStandardItem *> items;
	QStandardItem * item;
	QStandardItem * parent;
	CModel * model;
	CPosition * position;
	CGLShaderWrapper * shader;

	for(int i = 0; i < model_list->size(); i++)
	{
		// First pull out the model parameters
		model = model_list->GetModel(i);
		items = LoadParametersHeader(QString("Model"), model);
		parent = items[0];
		TreeModel->appendRow(items);
		LoadParameters(parent, model);

		// Now for the Position Parameters
		position = model->GetPosition();
		items = LoadParametersHeader(QString("Position"), position);
		item = items[0];
		parent->appendRow(items);
		LoadParameters(item, position);

		// Lastly for the shader:
		shader = model->GetShader();
		items = LoadParametersHeader(QString("Shader"), shader);
		item = items[0];
		parent->appendRow(items);
		LoadParameters(item, shader);

	}

	// Set the model
	ui.treeModels->setModel(TreeModel);
	ui.treeModels->setHeaderHidden(false);
	ui.treeModels->resizeColumnToContents(1);

	// Now connect the slot
	connect(TreeModel, SIGNAL(parameterUpdated(void)), this, SLOT(render(void)));
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
    CGLWidget *widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
    if (widget)
    {
        widget->stopRendering();
        delete widget;
    }
}

void cmaingui::LoadParameters(QStandardItem * parent, CParameters * parameters)
{
	for(int j = 0; j < parameters->GetNParams(); j++)
	{
		QList<QStandardItem *> items;
		QStandardItem * item;

		// First the name
		item = new QStandardItem(QString::fromStdString(parameters->GetParamName(j)));
		items << item;

		// Now the checkbox
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setCheckable(true);
		if(parameters->IsFree(j))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
		items << item;

		// Lastly the value:
		item = new CParameterItem(parameters, j);
		item->setEditable(true);
		item->setData(QVariant((double)parameters->GetParam(j)), Qt::DisplayRole);
		items << item;

		parent->appendRow(items);
	}
}

QList<QStandardItem *> cmaingui::LoadParametersHeader(QString name, CParameters * param_base)
{
	QList<QStandardItem *> items;
	QStandardItem * item;
	item = new QStandardItem(name);
	items << item;
	item = new QStandardItem(QString(""));
	items << item;
	item = new QStandardItem(QString::fromStdString(param_base->GetName()));
	items << item;

	return items;
}

void cmaingui::render()
{
    CGLWidget * widget = (CGLWidget*) ui.mdiArea->activeSubWindow()->widget();
    if(widget)
    {
    	widget->EnqueueOperation(GLT_RenderModels);
    }
}

void cmaingui::subwindowSelected(QMdiSubWindow * window)
{
	//ui.treeModels
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

//void cmaingui::model_clicked(const QModelIndex & index)
//{
//	QString text = ui.treeModels->currentIndex().data().toString();
//	printf("Label: %s \n", text.toStdString().c_str());
//}
