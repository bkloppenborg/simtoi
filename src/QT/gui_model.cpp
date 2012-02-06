#include "gui_model.h"

gui_model::gui_model(QWidget *parent)
    : QDialog(parent)
{
	ui.setupUi(this);
}

gui_model::~gui_model()
{

}

void gui_model::SetFeatureTypes(vector< pair<int, string> > feature_info)
{
//	SetupComboOptions(ui.listFeatures, feature_info);
}

void gui_model::SetModelTypes(vector< pair<eModels, string> > model_info)
{
	gui_model_ns::SetupComboOptions(ui.cboModels, model_info);
}

void gui_model::SetPositionTypes(vector< pair<int, string> > position_info)
{
	gui_model_ns::SetupComboOptions(ui.cboPositions, position_info);
}

void gui_model::SetShaderTypes(vector< pair<CGLShaderList::ShaderTypes, string> > shader_info)
{
	gui_model_ns::SetupComboOptions(ui.cboShaders, shader_info);
}

