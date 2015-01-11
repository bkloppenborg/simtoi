/*
 * wMinimizer.cpp
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#include "wMinimizer.h"
#include <QDir>
#include <QMessageBox>

#include "guiCommon.h"
#include "CMinimizerFactory.h"
#include "CMinimizerThread.h"
#include "CGLWidget.h"

wMinimizer::wMinimizer(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	// Setup the combo boxes.
	auto minimizers = CMinimizerFactory::Instance();
	guiCommon::setOptions(this->cboMinimizers, minimizers.GetMinimizerList());

	// Setup the text boxes
	mSaveDirectory = "/tmp/model";
	textSaveFolder->setText(mSaveDirectory.c_str());

	setEnabled(false);
}

wMinimizer::~wMinimizer()
{
//	mAnimator.stop();
//	mAnimator.wait();
}



/// Sets the current widget. Connects necessary signals and slots.
void wMinimizer::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

//	mAnimator.stop();
//	mAnimator.setGLWidget(gl_widget);

	toggleButtons();
}

/// Automatically (de)acitvate buttons depending on the status of various properties
/// of this widget.
void wMinimizer::toggleButtons()
{
	this->setEnabled(false);

	if(mGLWidget)
		this->setEnabled(true);
}
//
///// Checks to see which buttons can be enabled/disabled.
//void gui_main::ButtonCheck()
//{
//	if(!mGLWidget)
//		return;
//
//	// Buttons for minimizer area
//	// Look up the minimizer's ID in the combo box, select it.
//	int id = this->cboMinimizers->findText(QString::fromStdString(mGLWidget->GetMinimizerID()));
//	if(id > -1)
//		this->cboMinimizers->setCurrentIndex(id);
//
//	// Toggle minimizer start/stop button
//	if(mGLWidget->GetMinimizerRunning())
//		this->btnMinimizerStartStop->setText("Stop");
//	else
//		this->btnMinimizerStartStop->setText("Start");
//
//}


void wMinimizer::startMinimizer(string minimizer_id, string save_directory)
{
	// Look up the minimizer
	auto minimizers = CMinimizerFactory::Instance();

	// stop any presently running minimization engine.
	if(mMinimizer && mMinimizer->isRunning())
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}

	if(mGLWidget->getNData() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("You must load data prior to starting a minimizer"));
		msgBox.exec();
		return;
	}

	if(mGLWidget->getNModels() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("You must define a model prior to starting a minimizer"));
		msgBox.exec();
		return;
	}

	// try to start the minimizer
	try
	{
		auto worker = mGLWidget->getWorker();

		mMinimizer = minimizers.CreateMinimizer(minimizer_id);
		mMinimizer->setSaveDirectory(save_directory);
		mMinimizer->Init(worker);
		mMinimizer->start();

		// the minimizer started, update the minimizer ID combo box
		int id = this->cboMinimizers->findText(QString::fromStdString(minimizer_id));
		if(id > -1)
			this->cboMinimizers->setCurrentIndex(id);
		// signal that the minimization started.
		minimizerStarted();

		connect(mMinimizer.get(), SIGNAL(finished()), this, SLOT(minimizerFinished()));
	}
	catch(runtime_error & e)
	{
		QMessageBox msgBox;
		msgBox.setText(QString("Could not start minimizer.\n") + QString(e.what()));
		msgBox.exec();
	}
}

void wMinimizer::minimizerStarted()
{
	btnStartStop->setText(QString("Stop"));
	emit started();
}

void wMinimizer::minimizerFinished()
{
	btnStartStop->setText(QString("Start"));
	emit finished();
}

/// Starts the minimizer
void wMinimizer::on_btnStartStop_clicked()
{
	/// TODO: Handle thread-execution exceptions that might be thrown.
	if(!mGLWidget)
		return;

	// Determine the save-file directory
	QString save_dir = textSaveFolder->text();
	// make the directory if it doesn't exist
	if(!QDir(save_dir).exists())
		QDir().mkdir(save_dir);

	mSaveDirectory = save_dir.toStdString();

	// If the minimizer is running, stop it.
	if(mMinimizer && mMinimizer->isRunning())
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}
	else // start a new minimizer.
	{
		string id = this->cboMinimizers->currentText().toStdString();
		startMinimizer(id, mSaveDirectory);
	}
}
