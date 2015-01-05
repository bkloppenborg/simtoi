/*
 * wDataEditor.cpp
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
 */

#include "wDataEditor.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QStandardItemModel>

#include "CGLWidget.h"

wDataEditor::wDataEditor(QWidget * parent)
	: QWidget(parent)
{
	this->setupUi(this);

	QStringList labels = QStringList();
	labels << "Filename" << "Quantity" << "JD Start" << "JD End" << "Mean JD" << "chi2r";
	mOpenFileModel.clear();
	mOpenFileModel.setColumnCount(6);
	mOpenFileModel.setHorizontalHeaderLabels(labels);
}

wDataEditor::~wDataEditor()
{

}

void wDataEditor::addData(CDataInfo data_info)
{
	QList<QStandardItem *> items;
	QString temp;

	unsigned int precision = 4;

	// filename
	temp = QString::fromUtf8(data_info.getFilename().c_str());
	items.append(new QStandardItem(temp));
	// number of data
	temp = QString::fromUtf8(data_info.getQuantityDescription().c_str());
	items.append(new QStandardItem(temp));
	// starting julian date
	temp = QString::number(data_info.mJDStart, 'f', precision);
	items.append(new QStandardItem(temp));
	// ending julian date
	temp = QString::number(data_info.mJDEnd, 'f', precision);
	items.append(new QStandardItem(temp));
	// mean julian date
	temp = QString::number(data_info.mJDMean, 'f', precision);
	items.append(new QStandardItem(temp));
	// chi2 (when applicable)
	temp = QString::fromUtf8("");
	items.append(new QStandardItem(temp));

	mOpenFileModel.appendRow(items);
}

void wDataEditor::dataRemoved(unsigned int index)
{
	mOpenFileModel.removeRow(index);
}

void wDataEditor::on_btnAddData_clicked(void)
{
	// Ensure there is a selected widget, if not immediately return.
    if(!mGLWidget)
    {
		QMessageBox msgBox;
		msgBox.setText("You must have an active model region before you can load data.");
		msgBox.exec();
    	return;
    }

    // Open a dialog, get a list of file that the user selected:
    QFileDialog dialog(this);
    dialog.setDirectory(mOpenDataDir);
    dialog.setFileMode(QFileDialog::ExistingFiles);

    // Now add in valid file types:
    QStringList filters = mGLWidget->GetFileFilters();
    dialog.setNameFilters(filters);

	if (dialog.exec())
	{
		// store the open data directory
		mOpenDataDir = dialog.directory();

		// get the files and tell the GLWidget to open them.
		QStringList filenames = dialog.selectedFiles();
		for(auto filename: filenames)
			mGLWidget->addData(filename.toUtf8().constData());
	}
}

/// Removes the current selected data set.
void wDataEditor::on_btnRemoveData_clicked()
{
	if(!mGLWidget)
		return;

    // Get the selected indicies:
    QModelIndexList list = this->treeOpenFiles->selectionModel()->selectedIndexes();

    QList<QModelIndex>::iterator it;
    int id = 0;
    for(it = --list.end(); it > list.begin(); it--)
    {
    	id = (*it).row();
    	mGLWidget->removeData(id);
    	mOpenFileModel.removeRow(id, QModelIndex());
    }
}

void wDataEditor::refreshTree()
{
	this->treeOpenFiles->setHeaderHidden(false);
	this->treeOpenFiles->setModel(&mOpenFileModel);
	this->treeOpenFiles->header()->setResizeMode(QHeaderView::ResizeToContents);
}

/// Sets the current widget. Connects necessary signals and slots.
void wDataEditor::setGLWidget(CGLWidgetPtr gl_widget)
{
	mGLWidget = gl_widget;

	// connect any non-automatic signal/slots
	connect(mGLWidget.get(), SIGNAL(dataAdded(CDataInfo)), this, SLOT(addData(CDataInfo)));

	toggleButtons();
	refreshTree();
}

void wDataEditor::toggleButtons()
{
	this->btnAddData->setEnabled(false);
	this->btnRemoveData->setEnabled(false);

	if(!mGLWidget)
		return;

	// Buttons for add/delete data
	this->btnAddData->setEnabled(true);
	if(mOpenFileModel.rowCount() > 0)
		this->btnRemoveData->setEnabled(true);
}
