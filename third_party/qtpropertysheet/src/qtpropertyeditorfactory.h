#ifndef QTPROPERTYEDITORFACTORY_H
#define QTPROPERTYEDITORFACTORY_H

#include <QObject>
#include <QMap>
#include "qtpropertytype.h"

class QWidget;
class QtProperty;
class QtPropertyEditor;

typedef QtPropertyEditor* (*QtPropertyEditorCreator)(QtProperty *property);

class QTPROPERTYSHEET_DLL QtPropertyEditorFactory : public QObject
{
    Q_OBJECT
public:
    explicit QtPropertyEditorFactory(QObject *parent);

    QtPropertyEditor* createPropertyEditor(QtProperty *property, QtPropertyType::Type type=QtPropertyType::NONE);

    QWidget* createEditor(QtProperty *property, QWidget *parent);

    void registerCreator(QtPropertyType::Type type, QtPropertyEditorCreator method);

    template <typename T>
    void registerCreator(QtPropertyType::Type type);

private:
    template <typename T>
    static QtPropertyEditor* internalCreator(QtProperty *property);

    typedef QMap<QtPropertyType::Type, QtPropertyEditorCreator> CreatorMap;
    CreatorMap      creators_;
};

template <typename T>
void QtPropertyEditorFactory::registerCreator(QtPropertyType::Type type)
{
    this->registerCreator(type, this->internalCreator<T>);
}

template <typename T>
QtPropertyEditor* QtPropertyEditorFactory::internalCreator(QtProperty *property)
{
    return new T(property);
}

#endif // QTPROPERTYEDITORFACTORY_H
