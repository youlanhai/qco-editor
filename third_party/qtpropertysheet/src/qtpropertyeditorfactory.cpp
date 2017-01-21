#include "qtpropertyeditorfactory.h"
#include "qtproperty.h"
#include "qtpropertyeditor.h"

#include <QWidget>

QtPropertyEditorFactory::QtPropertyEditorFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_CREATOR(TYPE, CLASS) \
    registerCreator<CLASS>(TYPE)

    REGISTER_CREATOR(QtPropertyType::INT, QtIntSpinBoxEditor);
    REGISTER_CREATOR(QtPropertyType::FLOAT, QtDoubleSpinBoxEditor);
    REGISTER_CREATOR(QtPropertyType::STRING, QtStringEditor);
    REGISTER_CREATOR(QtPropertyType::ENUM, QtEnumEditor);
    REGISTER_CREATOR(QtPropertyType::BOOL, QtBoolEditor);
    REGISTER_CREATOR(QtPropertyType::COLOR, QtColorEditor);
    REGISTER_CREATOR(QtPropertyType::FLAG, QtFlagEditor);
    REGISTER_CREATOR(QtPropertyType::FILE, QtFileEditor);
    REGISTER_CREATOR(QtPropertyType::DYNAMIC_ITEM, QtDynamicItemEditor);
    REGISTER_CREATOR(QtPropertyType::ENUM_PAIR, QtEnumPairEditor);
    REGISTER_CREATOR(QtPropertyType::FLOAT_LIST, QtFloatListEditor);

#undef QtSpinBoxEditor
}

QWidget* QtPropertyEditorFactory::createEditor(QtProperty *property, QWidget *parent)
{
    QtPropertyEditor *propertyEditor = createPropertyEditor(property, property->getType());
    if(propertyEditor != NULL)
    {
        QWidget *widget = propertyEditor->createEditor(parent, this);
        if(widget != NULL)
        {
             QObject::connect(widget, SIGNAL(destroyed(QObject*)), propertyEditor, SLOT(slotEditorDestory(QObject*)));
        }
        return widget;
    }
    return NULL;
}

QtPropertyEditor* QtPropertyEditorFactory::createPropertyEditor(QtProperty *property, QtPropertyType::Type type)
{
    if(type == QtPropertyType::NONE)
    {
        type = property->getType();
    }

    QtPropertyEditorCreator method = creators_.value(type);
    if(method != NULL)
    {
        return method(property);
    }
    return NULL;
}

void QtPropertyEditorFactory::registerCreator(QtPropertyType::Type type, QtPropertyEditorCreator method)
{
    creators_[type] = method;
}
