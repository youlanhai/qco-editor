#include "editor_setting.h"
#include "ui_editorsetting.h"
#include "framework.h"
#include "canvas/canvas_mgr.h"
#include "project_mgr.h"
#include "user_configure.h"

#include "properties/property_group.h"
#include "properties/property_default.h"

#include "tools/log_tool.h"
#include "tools/json_tools.h"
#include "tools/file_tools.h"

#include <qtproperty.h>
#include <qtpropertyeditorfactory.h>
#include <qttreepropertybrowser.h>

#include <QFileDialog>

#include <base/CCDirector.h>

using namespace Editor;
const char *SettingKeys[] = {"EditorBasicSettings", "EditorCanvasSettings", nullptr};

static void applyConfigToProperty(QtProperty *property, const JsonValue &value)
{
    if(!value.isDict())
    {
        return;
    }

    const mjson::Dict &dict = value.refDict();
    for(const mjson::NodePair &pair : dict)
    {
        QVariant value;
        json2tvalue(value, pair.value);
        property->setChildValue(json2qstring(pair.key), value);
    }
}

EditorSetting::EditorSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditorSetting)
{
    ui->setupUi(this);

    editorFactor_ = new QtPropertyEditorFactory(this);

    propertyBrowser_ = new QtTreePropertyBrowser(this);
    propertyBrowser_->init(ui->widget, editorFactor_);

    for(const char **it = SettingKeys; *it; ++it)
    {
        const char *name = *it;
        PropertyGroup *group = PropertyGroupMgr::instance()->findGroup(name);
        if(group)
        {
            QtProperty *property = group->getProperty();
            properties_.push_back(property);

            JsonValue value = PropertyDefault::instance()->name2config(name);
            applyConfigToProperty(property, value);

            propertyBrowser_->addProperty(property);
            propertyBrowser_->setExpanded(property, true);

            connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onPropertyValueChange(QtProperty*)));
        }
        else
        {
            LOG_ERROR("Failed to find propery group %s", name);
        }
    }

    const JsonValue &config = UserConfigure::instance()->getConfig();
    foreach(QtProperty *property, properties_)
    {
        applyConfigToProperty(property, config);
    }
}

EditorSetting::~EditorSetting()
{
    delete ui;
}

void EditorSetting::onPropertyValueChange(QtProperty *property)
{
    std::string key = property->getName().toStdString();
    JsonValue value;
    tvalue2json(value, property->getValue());

    if(UserConfigure::instance()->applyConfig(key, value))
    {
        UserConfigure::instance()->setConfig(key, value);
        UserConfigure::instance()->save();
    }
}
