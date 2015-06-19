/*
 * guiModel.cpp
 *
 *  Created on: Aug 21, 2014
 *      Author: bkloppenborg
 */

#include "guiCommon.h"

#include "CModel.h"
#include "CModelFactory.h"
#include "CPositionFactory.h"
#include "CShaderFactory.h"
#include "CFeature.h"
#include "CFeatureFactory.h"
#include "guiModelEditor.h"

guiModelEditor::guiModelEditor()
{
	this->setupUi(this);
	initUi();
}

/// Creates a model gui with properties populated to match the input model
guiModelEditor::guiModelEditor(CModelPtr model)
{
	this->setupUi(this);
	initUi();

	mModel = model;

	// get information about the model
	QString model_name = QString::fromStdString(mModel->name());
	QString position_name = QString::fromStdString(mModel->GetPosition()->name());
	QString shader_id = QString::fromStdString(mModel->GetShader()->ID());

	const vector<CFeaturePtr> features = mModel->GetFeatures();

	guiCommon::setSelectedValue(cboModels, model_name);
	guiCommon::setSelectedValue(cboPositions, position_name);
	guiCommon::setSelectedValue(cboShaders, shader_id);

	QString feature_name;
	for(auto feature: features)
	{
		feature_name = QString::fromStdString(feature->name());
		listFeatures->addItem(feature_name);
	}
}

guiModelEditor::~guiModelEditor() {
	// TODO Auto-generated destructor stub
}

/// Returns an instance of a CModelPtr as defined by the properties set on the GUI.
CModelPtr guiModelEditor::getModel()
{
	string model_id = cboModels->currentText().toStdString();
	string position_id = cboPositions->currentText().toStdString();
	string shader_id = cboShaders->currentText().toStdString();

	if(mModel)	// the model exists, update changes
	{
		if(model_id != mModel->ID())
		{
			// Save the position, shader, and features
			CPositionPtr position = mModel->GetPosition();
			CShaderPtr shader = mModel->GetShader();
			auto features = mModel->GetFeatures();

			// create the model
			mModel = CModelFactory::getInstance().create(model_id);
			// restore the position, shader, and features.
			mModel->SetPositionModel(position);
			mModel->SetShader(shader);
			mModel->SetFeatures(features);
		}

		if(position_id != mModel->GetPosition()->ID())
			mModel->SetPositionModel(position_id);

		if(shader_id != mModel->GetShader()->ID())
			mModel->SetShader(shader_id);

	}
	else	// construct a new model from scratch
	{
		mModel = CModelFactory::getInstance().create(model_id);
		mModel->SetPositionModel(position_id);
		mModel->SetShader(shader_id);

		QListWidgetItem * item;
		string feature_id;
		for ( int i = 0 ; i < listFeatures->count() ; ++i )
		{
			item = listFeatures->item(i);
			feature_id = item->data(Qt::DisplayRole).toString().toStdString();
			mModel->AddFeature(feature_id);
		}

	}

	return mModel;
}

/// Initalizes the UI with values found elsewhere in SIMTOI.
void guiModelEditor::initUi()
{
	auto shaders = CShaderFactory::Instance();

	guiCommon::setOptions(cboModels, CModelFactory::getInstance().getNames());
	guiCommon::setOptions(cboPositions, CPositionFactory::getInstance().getNames());
	guiCommon::setOptions(cboShaders, shaders.GetShaderList());
	guiCommon::setOptions(cboFeatures, CFeatureFactory::getInstance().getNames());
}

/// Adds a feature to the model
void guiModelEditor::on_btnFeatureAdd_clicked()
{
	QString feature_id = cboFeatures->currentText();
	listFeatures->addItem(feature_id);
}

/// Deletes all features from the model
void guiModelEditor::on_btnFeatureRemove_clicked()
{
	qDeleteAll(listFeatures->selectedItems());
}
