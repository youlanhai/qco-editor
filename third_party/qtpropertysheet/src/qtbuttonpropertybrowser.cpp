#include "qtbuttonpropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertyeditorfactory.h"
#include "qtpropertybrowserutils.h"
#include "qtbuttonpropertyitem.h"

#include <cassert>
#include <QApplication>
#include <QHBoxLayout>
#include <QGridLayout>

QtButtonPropertyBrowser::QtButtonPropertyBrowser(QObject *parent)
    : QtPropertyBrowser(parent)
    , editorFactory_(NULL)
    , rootItem_(NULL)
    , mainView_(NULL)
{

}

QtButtonPropertyBrowser::~QtButtonPropertyBrowser()
{
    removeAllProperties();

    if(rootItem_ != NULL)
    {
        delete rootItem_;
    }
}

bool QtButtonPropertyBrowser::init(QWidget *parent, QtPropertyEditorFactory *factory)
{
    editorFactory_ = factory;

    QVBoxLayout *parentLayout = new QVBoxLayout();
    parentLayout->setMargin(4);
    parentLayout->setSpacing(0);
    parent->setLayout(parentLayout);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(4);

    QWidget *mainView_ = new QWidget(parent);
    mainView_->setLayout(mainLayout);
    parent->setFocusProxy(mainView_);
    parentLayout->addWidget(mainView_);

    QLayoutItem *item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    parentLayout->addItem(item);

    rootItem_ = new QtButtonPropertyItem();
    rootItem_->setLayout(mainLayout);

    connect(mainView_, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewDestroy(QObject*)));
    return true;
}

QWidget* QtButtonPropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    if(editorFactory_ != NULL)
    {
        return editorFactory_->createEditor(property, parent);
    }
    return NULL;
}

QtProperty* QtButtonPropertyBrowser::itemToProperty(QtButtonPropertyItem* item)
{
    return item->property();
}

void QtButtonPropertyBrowser::addProperty(QtProperty *property)
{
    if(property2items_.contains(property))
    {
        return;
    }

    addProperty(property, rootItem_);
}

void QtButtonPropertyBrowser::addProperty(QtProperty *property, QtButtonPropertyItem *parentItem)
{
    assert(parentItem != NULL);

    QtButtonPropertyItem *item = NULL;
    if(property->isSelfVisible())
    {
        item = new QtButtonPropertyItem(property, parentItem, editorFactory_);
        parentItem->addChild(item);
        parentItem = item;
    }
    property2items_[property] = item;

    connect(property, SIGNAL(signalPropertyInserted(QtProperty*,QtProperty*)), this, SLOT(slotPropertyInsert(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalPropertyRemoved(QtProperty*,QtProperty*)), this, SLOT(slotPropertyRemove(QtProperty*,QtProperty*)));
    connect(property, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotPropertyValueChange(QtProperty*)));
    connect(property, SIGNAL(signalPropertyChange(QtProperty*)), this, SLOT(slotPropertyPropertyChange(QtProperty*)));

    // add it's children finaly.
    foreach(QtProperty *child, property->getChildren())
    {
        addProperty(child, parentItem);
    }
}

void QtButtonPropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end())
    {
        QtButtonPropertyItem *item = it.value();
        if(item != NULL)
        {
            item->removeFromParent();
        }

        property2items_.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->getChildren())
        {
            removeProperty(child);
        }

        // then remove this QtButtonPropertyItem
        if(item != NULL)
        {
            deleteItem(item);
        }
    }
}

void QtButtonPropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = property2items_.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    property2items_.clear();
}

void QtButtonPropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QtButtonPropertyItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}

void QtButtonPropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/)
{
    removeProperty(property);
}

void QtButtonPropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
//    QtButtonPropertyItem *item = property2items_.value(property);
//    if(item != NULL)
//    {
//        item->setText(1, property->getValueString());
//        item->setIcon(1, property->getValueIcon());
//    }
}

void QtButtonPropertyBrowser::slotPropertyPropertyChange(QtProperty *property)
{
    QtButtonPropertyItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setTitle(property->getTitle());
        item->setVisible(property->isVisible());
    }
}

void QtButtonPropertyBrowser::slotViewDestroy(QObject *p)
{
    removeAllProperties();
}

void QtButtonPropertyBrowser::deleteItem(QtButtonPropertyItem *item)
{
    delete item;
}

bool QtButtonPropertyBrowser::isExpanded(QtProperty *property)
{
    QtButtonPropertyItem *item = property2items_.value(property);
    if(item != NULL)
    {
        return item->isExpanded();
    }
    return false;
}

void QtButtonPropertyBrowser::setExpanded(QtProperty *property, bool expand)
{
    QtButtonPropertyItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setExpanded(expand);
    }
}
