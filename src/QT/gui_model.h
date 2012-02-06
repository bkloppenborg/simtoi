#ifndef GUI_ADDMODEL_H
#define GUI_ADDMODEL_H

#include <QtGui/QWidget>
#include "ui_gui_model.h"
#include <QDialog>
#include <utility>
#include <vector>
#include "CModelList.h"
#include "CGLShaderList.h"

using namespace std;

class gui_model : public QDialog
{
    Q_OBJECT

public:
    gui_model(QWidget *parent = 0);
    ~gui_model();

    void SetFeatureTypes(vector< pair<int, string> > feature_info);
    void SetModelTypes(vector< pair<eModels, string> > model_info);
    void SetPositionTypes(vector< pair<int, string> > position_info);
    void SetShaderTypes(vector< pair<CGLShaderList::ShaderTypes, string> > shader_info);

private:
    //void SetupComboOptions(QComboBox * combo_box, vector< pair<int, string> > values);

private:
    Ui::gui_addmodelClass ui;
};

namespace gui_model_ns
{
template <typename T>
void SetupComboOptions(QComboBox * combo_box, vector< pair<T, string> > values)
{
	QString tmp_str = "";
	QVariant tmp_val = "";

	for(int i = 0; i < values.size(); i++)
	{
		tmp_val = QVariant(int(values[i].first));
		tmp_str = QString::fromStdString(values[i].second);
		combo_box->addItem(tmp_str, tmp_val);
	}
}

template <typename T>
T InterpretComboValue(int value)
{
	if(value < T::LAST_VALUE)
		return reinterpret_cast<T>(value);

	throw "Exception: Value out of range.";
	return 0;
}

}
#endif // GUI_ADDMODEL_H
