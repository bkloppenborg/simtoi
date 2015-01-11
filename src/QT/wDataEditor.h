/*
 * wDataEditor.h
 *
 *  Created on: Jan 4, 2015
 *      Author: bkloppenborg
 */

#ifndef WDATA_EDITOR_H_
#define WDATA_EDITOR_H_

#include "ui_wDataEditor.h"
#include <QWidget>
#include <QDir>
#include <QStandardItemModel>
#include <QStringList>
#include <memory>

#include "CGLWidget.h"
#include "CDataInfo.h"

using namespace std;

class CDataInfo;

class wDataEditor : public QWidget, public Ui::wDataEditor {
	Q_OBJECT

protected:
	CGLWidget * mGLWidget;
	QDir mOpenDataDir;
    QStandardItemModel mOpenFileModel;

public:
	wDataEditor(QWidget * parent = 0);
	virtual ~wDataEditor();

	void setGLWidget(CGLWidget * gl_widget);

	void openData(QStringList & filenames);

	void toggleButtons();
	void refreshTree();

private slots:

	void on_btnAddData_clicked();
	void on_btnRemoveData_clicked();

	void addData(CDataInfo data_info);
	void dataRemoved(unsigned int index);
};

#endif /* WDATA_EDITOR_H_ */
