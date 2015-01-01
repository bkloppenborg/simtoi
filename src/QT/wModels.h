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

#include "ui_wModels.h"

using namespace std;

class CGLWidget;
typedef shared_ptr<CGLWidget> CGLWidgetPtr;

class wModels : public QWidget, public Ui::wModels {
	Q_OBJECT

protected:
	CGLWidgetPtr mGLWidget;

public:
    wModels(QWidget * parent = 0);
    wModels(CGLWidgetPtr gl_widget, QWidget * parent = 0);
	virtual ~wModels();

	void setGLWidget(CGLWidgetPtr gl_widget);

public:
	void ButtonCheck();
	void TreeCheck();

private slots:
    void on_btnAddModel_clicked();
    void on_btnEditModel_clicked();
    void on_btnRemoveModel_clicked();


};

#endif /* WMODELS_H_ */
