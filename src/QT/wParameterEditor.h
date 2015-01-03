/*
 * wAnimation.h
 *
 *  Created on: Aug 26, 2014
 *      Author: bkloppenborg
 */

#ifndef WMODELS_H_
#define WMODELS_H_

#include <QWidget>
#include <memory>
#include <QList>
#include <QStandardItem>

#include "ui_wParameterEditor.h"

#include "CParameterItem.h"
#include "CParameterMap.h"
#include "CTreeModel.h"


using namespace std;

class CGLWidget;
typedef shared_ptr<CGLWidget> CGLWidgetPtr;

class wParameterEditor : public QWidget, public Ui::wParameterEditor {
	Q_OBJECT

protected:
	CGLWidgetPtr mGLWidget;
    CTreeModel mTreeModel;

public:
    wParameterEditor(QWidget * parent = 0);
	virtual ~wParameterEditor();

	void setGLWidget(CGLWidgetPtr gl_widget);

public:
	void buildTree();
	void toggleButtons();
	unsigned int getSelectedModelIndex();
	void refreshTree();

    static void LoadParameters(QStandardItem * parent_widget, CParameterMap * param_map);
    static QList<QStandardItem *> LoadParametersHeader(QString name, CParameterMap * param_map);

private slots:
    void on_btnAddModel_clicked();
    void on_btnEditModel_clicked();
    void on_btnRemoveModel_clicked();

    void modelUpdated();
};

#endif /* WMODELS_H_ */
