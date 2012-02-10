#ifndef GUI_ADDMODEL_H
#define GUI_ADDMODEL_H

#include <QtGui/QWidget>
#include "ui_gui_model.h"
#include <QDialog>
#include <utility>
#include <vector>
#include "CModelList.h"
#include "CGLShaderList.h"
#include "CModelList.h"

using namespace std;

class gui_model : public QDialog
{
    Q_OBJECT
private:
    Ui::gui_addmodelClass ui;

protected:
    CGLShaderList::ShaderTypes mShaderType;
    CModelList::ModelTypes mModelType;
    CPosition::PositionTypes mPositionType;

public:
    gui_model(QWidget *parent = 0);
    ~gui_model();

    CModelList::ModelTypes GetModelType() { return mModelType; };
    CGLShaderList::ShaderTypes GetShaderType() { return mShaderType; };
    CPosition::PositionTypes GetPositionType() { return mPositionType; };


    void SetFeatureTypes(vector< pair<int, string> > feature_info);
    void SetModelTypes(vector< pair<CModelList::ModelTypes, string> > model_info);
    void SetPositionTypes(vector< pair<CPosition::PositionTypes, string> > position_info);
    void SetShaderTypes(vector< pair<CGLShaderList::ShaderTypes, string> > shader_info);

    public slots:
    void ValuesAccepted();

};

#endif // GUI_ADDMODEL_H
