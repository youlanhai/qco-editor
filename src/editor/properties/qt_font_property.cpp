#include "qt_font_property.h"
#include "runtime/resource_mapping.h"

#include <qtproperty.h>
#include <qtattributename.h>
#include <QComboBox>

namespace Editor
{

    QtFontEditor::QtFontEditor(QtProperty *property)
        : QtEnumPairEditor(property)
    {
        enumNames_.clear();
        const StringMapping::StringMap &values = ResourceMapping::instance()->font.values();
        for(const auto & pair: values)
        {
            enumNames_.push_back(QString::fromStdString(pair.first));
        }
        enumNames_.sort(Qt::CaseInsensitive);

        enumValues_.clear();
        foreach(const QString &s, enumNames_)
        {
            enumValues_.push_back(s);
        }

        index_ = enumValues_.indexOf(property->getValue());
    }

    void QtFontEditor::slotSetAttribute(QtProperty *property, const QString &name)
    {
        if(name == QtAttributeName::EnumName)
        {
            editor_->clear();
            editor_->addItems(enumNames_);
        }
    }
}
