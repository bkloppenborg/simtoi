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
	textSaveFolder->setText("/tmp/model");

	setEnabled(false);
}

wMinimizer::~wMinimizer()
{
	if(mMinimizer)
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}
}

/// Sets the current widget. Connects necessary signals and slots.
void wMinimizer::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

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

/// Sets the desired save directory. Creates the directory if it does not exist.
void wMinimizer::setSaveDirectory(const string & save_directory)
{
	// verify the save directory exists, if not create it
	QString q_save_directory = QString::fromStdString(save_directory);
	if(!QDir(q_save_directory).exists())
	{
		QDir().mkdir(q_save_directory);
	}

	textSaveFolder->setText(q_save_directory);
}

/// Starts a minimization
///
/// In order to start the minimization engine, the following must hold
///  * At least one data file is loaded
///	 * At least one model exists
/// If one of these is not true, a dialog will be displayed to the user and
/// no minimization engine will be started.
///
/// \param minimizer_id The desired minimizer. See CMinimizerFactory for string names
/// \param save_directory The directory in which temporary output data should be saved.
void wMinimizer::startMinimizer(const string & minimizer_id, const string & save_directory)
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
		setSaveDirectory(save_directory);

		auto worker = mGLWidget->getWorker();

		mMinimizer = minimizers.CreateMinimizer(minimizer_id);
		mMinimizer->setSaveDirectory(save_directory);
		mMinimizer->Init(worker);
		mMinimizer->start();

		// the minimizer started, update the minimizer ID combo box
		QString q_minimizer_id = QString::fromStdString(minimizer_id);
		int id = this->cboMinimizers->findText(q_minimizer_id);
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

/// \brief Run specific operations when a minimizer is started.
///
/// Toggles the start/stop button and emits the `started` signal.
void wMinimizer::minimizerStarted()
{
	btnStartStop->setText(QString("Stop"));
	emit started();
}

/// \brief Run specific operations when a minimizer has finished.
///
/// Toggles the start/stop button and emits the `finished` signal.
void wMinimizer::minimizerFinished()
{
	btnStartStop->setText(QString("Start"));
	emit finished();
}

///	\brief Start or stop the minimization engine when btnStartStop is clicked
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

	string save_directory = save_dir.toStdString();

	// If the minimizer is running, stop it.
	if(mMinimizer && mMinimizer->isRunning())
	{
		mMinimizer->stop();
		mMinimizer->wait();
	}
	else // start a new minimizer.
	{
		string id = this->cboMinimizers->currentText().toStdString();
		startMinimizer(id, save_directory);
	}
}
