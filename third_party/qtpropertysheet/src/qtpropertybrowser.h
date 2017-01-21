#ifndef QT_PROPERTY_BROWSER_H
#define QT_PROPERTY_BROWSER_H

#include "qtpropertyconfig.h"
#include <QObject>

class QWidget;
class QtProperty;
class QtPropertyEditorFactory;


class QTPROPERTYSHEET_DLL QtPropertyBrowser : public QObject
{
    Q_OBJECT
public:
    QtPropertyBrowser(QObject *parent);
    ~QtPropertyBrowser();

    virtual bool init(QWidget *parent, QtPropertyEditorFactory *factory) = 0;

    virtual void addProperty(QtProperty *property) = 0;
    virtual void removeProperty(QtProperty *property) = 0;
    virtual void removeAllProperties() = 0;

    virtual bool isExpanded(QtProperty *property) = 0;
    virtual void setExpanded(QtProperty *property, bool expand) = 0;
};

#endif // QT_PROPERTY_BROWSER_H
