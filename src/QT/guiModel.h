/*
 * guiModel.h
 *
 *  Created on: Aug 21, 2014
 *      Author: bkloppenborg
 */

#ifndef GUIMODEL_H_
#define GUIMODEL_H_

#include <QDialog>
#include <memory>

#include "ui_guiModel.h"

using namespace std;

class CModel;
typedef shared_ptr<CModel> CModelPtr;

class guiModel : public QDialog, public Ui::guiModel {
	Q_OBJECT

protected:
	CModelPtr mModel;

public:
	guiModel();
	guiModel(CModelPtr model);
	virtual ~guiModel();

	CModelPtr getModel();

protected:
	void initUi();

public slots:
	void on_btnFeatureAdd_clicked();
	void on_btnFeatureRemove_clicked();
};

#endif /* GUIMODEL_H_ */
