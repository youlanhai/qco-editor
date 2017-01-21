#include "qttreepropertybrowser.h"
#include "qtproperty.h"
#include "qtpropertytreeview.h"
#include "qtpropertytreedelegate.h"
#include "qtpropertyeditorfactory.h"
#include "qtpropertybrowserutils.h"

#include <cassert>
#include <QTreeWidget>
#include <QApplication>
#include <QItemDelegate>
#include <QHBoxLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QHeaderView>
#include <QLineEdit>

namespace
{
const int PropertyDataIndex = Qt::UserRole + 1;
}

QtTreePropertyBrowser::QtTreePropertyBrowser(QObject *parent)
    : QtPropertyBrowser(parent)
    , editorFactory_(NULL)
    , treeWidget_(NULL)
    , delegate_(NULL)
{

}

QtTreePropertyBrowser::~QtTreePropertyBrowser()
{
    removeAllProperties();
}

bool QtTreePropertyBrowser::init(QWidget *parent, QtPropertyEditorFactory *factory)
{
    editorFactory_ = factory;

    QHBoxLayout *layout = new QHBoxLayout(parent);
    layout->setMargin(0);

    treeWidget_ = new QtPropertyTreeView(parent);
    treeWidget_->setEditorPrivate(this);
    treeWidget_->setIconSize(QSize(18, 18));
    layout->addWidget(treeWidget_);
    parent->setFocusProxy(treeWidget_);

    treeWidget_->setColumnCount(2);
    QStringList labels;
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Property"));
    labels.append(QCoreApplication::translate("QtTreePropertyBrowser", "Value"));
    treeWidget_->setHeaderLabels(labels);
    treeWidget_->setAlternatingRowColors(true);
    treeWidget_->setEditTriggers(QAbstractItemView::EditKeyPressed);

    delegate_ = new QtPropertyTreeDelegate(parent);
    delegate_->setEditorPrivate(this);
    treeWidget_->setItemDelegate(delegate_);

    //treeWidget_->header()->setMovable(false);
    //treeWidget_->header()->setResizeMode(QHeaderView::Stretch);

    expandIcon_ = QtPropertyBrowserUtils::drawIndicatorIcon(treeWidget_->palette(), treeWidget_->style());

    connect(treeWidget_, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(slotCurrentTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
    connect(treeWidget_, SIGNAL(destroyed(QObject*)), this, SLOT(slotTreeViewDestroy(QObject*)));
    return true;
}

bool QtTreePropertyBrowser::lastColumn(int column)
{
    return treeWidget_->header()->visualIndex(column) == treeWidget_->columnCount() - 1;
}

QColor QtTreePropertyBrowser::calculatedBackgroundColor(QtProperty *property)
{
    if(property->getParent() != NULL)
    {
        int index = property->getParent()->indexChild(property);
        return QColor(index % 2 ? Qt::blue : Qt::white);
    }

    if(dynamic_cast<QtGroupProperty*>(property) != 0)
    {
        return QColor(Qt::gray);
    }
    return QColor(Qt::white);
}

QWidget* QtTreePropertyBrowser::createEditor(QtProperty *property, QWidget *parent)
{
    if(editorFactory_ != NULL)
    {
        return editorFactory_->createEditor(property, parent);
    }
    return NULL;
}

QTreeWidgetItem* QtTreePropertyBrowser::getEditedItem()
{
    return delegate_->editedItem();
}

QTreeWidgetItem* QtTreePropertyBrowser::indexToItem(const QModelIndex &index)
{
    return treeWidget_->indexToItem(index);
}

void QtTreePropertyBrowser::slotCurrentTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)
{

}

QtProperty* QtTreePropertyBrowser::indexToProperty(const QModelIndex &index)
{
    return itemToProperty(treeWidget_->indexToItem(index));
}

QtProperty* QtTreePropertyBrowser::itemToProperty(QTreeWidgetItem* item)
{
    if(item != NULL)
    {
        quintptr ptr = item->data(0, PropertyDataIndex).value<quintptr>();
        return reinterpret_cast<QtProperty*>(ptr);
    }
    return NULL;
}

void QtTreePropertyBrowser::addProperty(QtProperty *property)
{
    if(property2items_.contains(property))
    {
        return;
    }

    addProperty(property, NULL);
}

void QtTreePropertyBrowser::addProperty(QtProperty *property, QTreeWidgetItem *parentItem)
{
    QTreeWidgetItem *item = NULL;
    if(property->isSelfVisible())
    {
        item = new QTreeWidgetItem();
        item->setText(0, property->getTitle());
        item->setData(0, PropertyDataIndex, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(property)));
        item->setToolTip(0, property->getToolTip());
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        if(parentItem != NULL)
        {
            parentItem->addChild(item);
        }
        else
        {
            treeWidget_->addTopLevelItem(item);
        }

        if(property->hasValue())
        {
            item->setIcon(1, property->getValueIcon());
            item->setText(1, property->getValueString());
        }
        else
        {
            item->setFirstColumnSpanned(true);
        }

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

void QtTreePropertyBrowser::removeProperty(QtProperty *property)
{
    Property2ItemMap::iterator it = property2items_.find(property);
    if(it != property2items_.end())
    {
        QTreeWidgetItem *item = it.value();
        property2items_.erase(it);
        disconnect(property, 0, this, 0);

        // remove it's children first.
        foreach(QtProperty *child, property->getChildren())
        {
            removeProperty(child);
        }

        // then remove this QTreeWidgetItem
        deleteTreeItem(item);
    }
}

void QtTreePropertyBrowser::removeAllProperties()
{
    QList<QtProperty*> properties = property2items_.keys();
    foreach(QtProperty *property, properties)
    {
        removeProperty(property);
    }
    property2items_.clear();
}

void QtTreePropertyBrowser::slotPropertyInsert(QtProperty *property, QtProperty *parent)
{
    QTreeWidgetItem *parentItem = property2items_.value(parent);
    addProperty(property, parentItem);
}

void QtTreePropertyBrowser::slotPropertyRemove(QtProperty *property, QtProperty * /*parent*/)
{
    removeProperty(property);
}

void QtTreePropertyBrowser::slotPropertyValueChange(QtProperty *property)
{
    QTreeWidgetItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setText(1, property->getValueString());
        item->setIcon(1, property->getValueIcon());
    }
}

void QtTreePropertyBrowser::slotPropertyPropertyChange(QtProperty *property)
{
    QTreeWidgetItem *item = property2items_.value(property);
    if(item != NULL)
    {
        item->setText(0, property->getTitle());
        item->setHidden(!property->isVisible());
    }
}

void QtTreePropertyBrowser::slotTreeViewDestroy(QObject *p)
{
    if(treeWidget_ == p)
    {
        treeWidget_ = NULL;
    }
}

void QtTreePropertyBrowser::deleteTreeItem(QTreeWidgetItem *item)
{
    if(treeWidget_)
    {
        delete item;
    }
}

bool QtTreePropertyBrowser::isExpanded(QtProperty *property)
{
    QTreeWidgetItem *treeItem = property2items_.value(property);
    if(treeItem != NULL)
    {
        return treeItem->isExpanded();
    }
    return false;
}

void QtTreePropertyBrowser::setExpanded(QtProperty *property, bool expand)
{
    QTreeWidgetItem *treeItem = property2items_.value(property);
    if(treeItem != NULL)
    {
        treeItem->setExpanded(expand);
    }
}
