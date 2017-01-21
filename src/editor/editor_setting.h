#ifndef EDITOR_SETTING_H
#define EDITOR_SETTING_H

#include "config.h"
#include <QDialog>
#include <smartjson/src/node.hpp>

namespace Ui {
class EditorSetting;
}

class QtProperty;
class QtPropertyBrowser;
class QtPropertyEditorFactory;

class EDITOR_DLL EditorSetting : public QDialog
{
    Q_OBJECT

public:
    explicit EditorSetting(QWidget *parent = 0);
    ~EditorSetting();

private slots:
    void onPropertyValueChange(QtProperty *property);

private:
    Ui::EditorSetting*  ui;

    QtPropertyBrowser*  propertyBrowser_;
    QtPropertyEditorFactory* editorFactor_;

    QVector<QtProperty*>    properties_;
};

#endif // EDITOR_SETTING_H
