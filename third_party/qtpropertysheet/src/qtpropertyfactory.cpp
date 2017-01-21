#include "qtpropertyfactory.h"
#include "qtproperty.h"


QtPropertyFactory::QtPropertyFactory(QObject *parent)
    : QObject(parent)
{
#define REGISTER_PROPERTY(TYPE, CLASS) \
    registerCreator(TYPE, new QtSimplePropertyCreator<CLASS>(TYPE, this))

    REGISTER_PROPERTY(QtPropertyType::LIST, QtListProperty);
    REGISTER_PROPERTY(QtPropertyType::DICT, QtDictProperty);
    REGISTER_PROPERTY(QtPropertyType::GROUP, QtGroupProperty);
    REGISTER_PROPERTY(QtPropertyType::ENUM, QtEnumProperty);
    REGISTER_PROPERTY(QtPropertyType::FLAG, QtFlagProperty);
    REGISTER_PROPERTY(QtPropertyType::BOOL, QtBoolProperty);
    REGISTER_PROPERTY(QtPropertyType::FLOAT, QtDoubleProperty);
    REGISTER_PROPERTY(QtPropertyType::COLOR, QtColorProperty);
    REGISTER_PROPERTY(QtPropertyType::DYNAMIC_LIST, QtDynamicListProperty);
    REGISTER_PROPERTY(QtPropertyType::DYNAMIC_ITEM, QtDynamicItemProperty);
    REGISTER_PROPERTY(QtPropertyType::FLOAT_LIST, QtFloatListProperty);

#undef REGISTER_PROPERTY
}

QtPropertyFactory::~QtPropertyFactory()
{
    for(CreatorMap::iterator it = propertyCreator_.begin(); it != propertyCreator_.end(); ++it)
    {
        delete it.value();
    }
}

QtProperty* QtPropertyFactory::createProperty(QtPropertyType::Type type)
{
    QtPropertyCreator *method = propertyCreator_.value(type);
    if(method != NULL)
    {
        return method->create();
    }
    
    // use default QtProperty
    return new QtProperty(type, this);
}

void QtPropertyFactory::registerCreator(QtPropertyType::Type type, QtPropertyCreator *method)
{
    QtPropertyCreator *p = propertyCreator_.value(type);
    if(p != NULL)
    {
        delete p;
    }
    propertyCreator_[type] = method;
}
