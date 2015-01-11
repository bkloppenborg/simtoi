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
	QString model_id = QString::fromStdString(mModel->getID());
	QString position_id = QString::fromStdString(mModel->GetPosition()->getID());
	QString shader_id = QString::fromStdString(mModel->GetShader()->getID());

	const vector<CFeaturePtr> features = mModel->GetFeatures();

	guiCommon::setSelectedValue(cboModels, model_id);
	guiCommon::setSelectedValue(cboPositions, position_id);
	guiCommon::setSelectedValue(cboShaders, shader_id);

	QString feature_id;
	for(auto feature: features)
	{
		feature_id = QString::fromStdString(feature->getID());
		listFeatures->addItem(feature_id);
	}
}

guiModelEditor::~guiModelEditor() {
	// TODO Auto-generated destructor stub
}

/// Returns an instance of a CModelPtr as defined by the properties set on the GUI.
CModelPtr guiModelEditor::getModel()
{
	auto factory = CModelFactory::Instance();

	string model_id = cboModels->currentText().toStdString();
	string position_id = cboPositions->currentText().toStdString();
	string shader_id = cboShaders->currentText().toStdString();

	if(mModel)	// the model exists, update changes
	{
		if(model_id != mModel->getID())
		{
			// Save the position, shader, and features
			CPositionPtr position = mModel->GetPosition();
			CShaderPtr shader = mModel->GetShader();
			auto features = mModel->GetFeatures();

			// create the model
			mModel = factory.CreateModel(model_id);
			// restore the position, shader, and features.
			mModel->SetPositionModel(position);
			mModel->SetShader(shader);
			mModel->SetFeatures(features);
		}

		if(position_id != mModel->GetPosition()->getID())
			mModel->SetPositionModel(position_id);

		if(shader_id != mModel->GetShader()->getID())
			mModel->SetShader(shader_id);

	}
	else	// construct a new model from scratch
	{
		mModel = factory.CreateModel(model_id);
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
	auto models = CModelFactory::Instance();
	auto positions = CPositionFactory::Instance();
	auto shaders = CShaderFactory::Instance();
	auto features = CFeatureFactory::Instance();

	guiCommon::setOptions(cboModels, models.GetModelList());
	guiCommon::setOptions(cboPositions, positions.GetPositionList());
	guiCommon::setOptions(cboShaders, shaders.GetShaderList());
	guiCommon::setOptions(cboFeatures, features.GetFeatureList());
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
