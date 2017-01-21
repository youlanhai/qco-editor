#include "qtproperty.h"
#include "qtpropertyfactory.h"
#include "qtpropertybrowserutils.h"
#include "qtattributename.h"
#include "qtpropertytype.h"

#include <QLocale>
#include <cassert>
#include <algorithm>

QtProperty::QtProperty(Type type, QtPropertyFactory *factory)
    : QObject(factory)
    , factory_(factory)
    , type_(type)
    , parent_(NULL)
    , visible_(true)
    , selfVisible_(true)
    , menuVisible_(false)
{

}

QtProperty::~QtProperty()
{
    emit signalPropertyRemoved(this, parent_);

    removeAllChildren(true);
    removeFromParent();
}

void QtProperty::setName(const QString &name)
{
    if(name != name_)
    {
        name_ = name;
        emit signalPropertyChange(this);
    }
}

const QString& QtProperty::getTitle() const
{
    return title_.isEmpty() ? name_ : title_;
}

void QtProperty::setTitle(const QString &title)
{
    if(title != title_)
    {
        title_ = title;
        emit signalPropertyChange(this);
    }
}

void QtProperty::setValue(const QVariant &value)
{
    if(value_ != value)
    {
        value_ = value;
        emit signalValueChange(this);
    }
}

void QtProperty::setVisible(bool visible)
{
    if(visible != visible_)
    {
        visible_ = visible;
        emit signalPropertyChange(this);
    }
}

QString QtProperty::getValueString() const
{
    return value_.toString();
}

QIcon QtProperty::getValueIcon() const
{
    return QIcon();
}

void QtProperty::setAttribute(const QString &name, const QVariant &value)
{
    attributes_[name] = value;

    emit signalAttributeChange(this, name);
}

QVariant QtProperty::getAttribute(const QString &name) const
{
    QtPropertyAttributes::const_iterator it = attributes_.constFind(name);
    if(it != attributes_.constEnd())
    {
        return *it;
    }
    return QVariant();
}

void QtProperty::addChild(QtProperty *child)
{
    assert(child->getParent() == NULL);
    children_.push_back(child);
    child->parent_ = this;

    onChildAdd(child);
    emit signalPropertyInserted(child, this);
}

void QtProperty::removeChild(QtProperty *child)
{
    assert(child->getParent() == this);
    QtPropertyList::iterator it = std::find(children_.begin(), children_.end(), child);
    if(it != children_.end())
    {
        child->parent_ = NULL;
        children_.erase(it);

        onChildRemove(child);
        emit signalPropertyRemoved(child, this);
    }
}

void QtProperty::removeFromParent()
{
    if(parent_ != NULL)
    {
        parent_->removeChild(this);
    }
}

void QtProperty::removeAllChildren(bool clean)
{
    QtPropertyList temp = children_;
    foreach(QtProperty *child, temp)
    {
        removeChild(child);

        if(clean)
        {
            delete child;
        }
    }
}

int QtProperty::indexChild(const QtProperty *child) const
{
    return children_.indexOf(const_cast<QtProperty*>(child));
}

QtProperty* QtProperty::findChild(const QString &name)
{
    foreach(QtProperty *child, children_)
    {
        if(child->getName() == name)
        {
            return child;
        }
    }
    return NULL;
}

void QtProperty::setChildValue(const QString &name, const QVariant &value)
{
    QtProperty *child = findChild(name);
    if(child != NULL)
    {
        child->setValue(value);
    }
}

void QtProperty::onChildAdd(QtProperty* /*child*/)
{

}

void QtProperty::onChildRemove(QtProperty* /*child*/)
{

}

/********************************************************************/
QtContainerProperty::QtContainerProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

void QtContainerProperty::onChildAdd(QtProperty *child)
{
    connect(child, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotChildValueChange(QtProperty*)));
}

void QtContainerProperty::onChildRemove(QtProperty *child)
{
    disconnect(child, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotChildValueChange(QtProperty*)));
}

/********************************************************************/
static void ensureSize(QVariantList &list, int size)
{
    while(list.size() < size)
    {
        list.push_back(QVariant());
    }
}

QtListProperty::QtListProperty(Type type, QtPropertyFactory *factory)
    : QtContainerProperty(type, factory)
{

}

void QtListProperty::setValue(const QVariant &value)
{
    if(value_ == value)
    {
        return;
    }

    QVariantList valueList = value.toList();
    ensureSize(valueList, children_.size());
    for(int i = 0; i < children_.size(); ++i)
    {
        children_[i]->setValue(valueList[i]);
    }
    value_ = valueList;

    emit signalValueChange(this);
}

QString QtListProperty::getValueString() const
{
    QString text;
    text += "[";
    foreach(QtProperty *child, children_)
    {
        text += child->getValueString();
        text += ", ";
    }
    text += "]";
    return text;
}

void QtListProperty::slotChildValueChange(QtProperty *child)
{
    int i = indexChild(child);
    if(i >= 0)
    {
        QVariantList valueList = value_.toList();
        ensureSize(valueList, children_.size());

        if(valueList[i] != child->getValue())
        {
            valueList[i] = child->getValue();

            value_ = valueList;
            emit signalValueChange(this);
        }
    }
}

/********************************************************************/
QtDictProperty::QtDictProperty(Type type, QtPropertyFactory *factory)
    : QtContainerProperty(type, factory)
{

}

void QtDictProperty::setValue(const QVariant &value)
{
    if(value_ == value)
    {
        return;
    }
    value_ = value;

    QVariantMap valueMap = value_.toMap();
    foreach (QtProperty *child, children_)
    {
        QVariant value = valueMap.value(child->getName());
        child->setValue(value);
    }

    emit signalValueChange(this);
}

void QtDictProperty::slotChildValueChange(QtProperty *property)
{
    QVariantMap valueMap = value_.toMap();
    QVariant oldValue = valueMap.value(property->getName());
    if(property->getValue() != oldValue)
    {
        valueMap[property->getName()] = property->getValue();

        value_ = valueMap;
        emit signalValueChange(this);
    }
}

/********************************************************************/
QtGroupProperty::QtGroupProperty(Type type, QtPropertyFactory *factory)
    : QtContainerProperty(type, factory)
{

}

void QtGroupProperty::setValue(const QVariant& /*value*/)
{

}

QtProperty* QtGroupProperty::findChild(const QString &name)
{
    QtProperty *result = NULL;
    foreach(QtProperty *child, children_)
    {
        if(name == child->getName())
        {
            result = child;
        }
        else if(child->getType() == QtPropertyType::GROUP)
        {
            result = child->findChild(name);
        }

        if(result != NULL)
        {
            break;
        }
    }
    return result;
}

void QtGroupProperty::setChildValue(const QString &name, const QVariant &value)
{
    foreach(QtProperty *child, children_)
    {
        if(child->getType() ==  QtPropertyType::GROUP)
        {
            child->setChildValue(name, value);
        }
        else if(name == child->getName())
        {
            child->setValue(value);
        }
    }
}

void QtGroupProperty::slotChildValueChange(QtProperty *property)
{
    // emit signal to listner directly.
    emit signalValueChange(property);
}


/********************************************************************/
QtEnumProperty::QtEnumProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QString QtEnumProperty::getValueString() const
{
    int index = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::EnumName).toStringList();
    if(index >= 0 && index < enumNames.size())
    {
        return enumNames[index];
    }
    return QString();
}

/********************************************************************/
QtFlagProperty::QtFlagProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QString QtFlagProperty::getValueString() const
{
    int value = value_.toInt();
    QStringList enumNames = attributes_.value(QtAttributeName::FlagName).toStringList();

    QStringList selected;
    for(int i = 0; i < enumNames.size(); ++i)
    {
        if(value & (1 << i))
        {
            selected.push_back(enumNames[i]);
        }
    }
    return selected.join('|');
}

/********************************************************************/
QtBoolProperty::QtBoolProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QString QtBoolProperty::getValueString() const
{
    return value_.toBool() ? "True" : "False";
}

QIcon QtBoolProperty::getValueIcon() const
{
    return QtPropertyBrowserUtils::drawCheckBox(value_.toBool());
}

/********************************************************************/
QtDoubleProperty::QtDoubleProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QString QtDoubleProperty::getValueString() const
{
    QVariant v = getAttribute(QtAttributeName::Decimals);
    int decimals = v.type() == QVariant::Int ? v.toInt() : 2;
    return QLocale::system().toString(value_.toDouble(), 'f', decimals);
}

/********************************************************************/
QtColorProperty::QtColorProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QString QtColorProperty::getValueString() const
{
    QColor color = QtPropertyBrowserUtils::variant2color(value_);
    return QtPropertyBrowserUtils::colorValueText(color);
}

QIcon QtColorProperty::getValueIcon() const
{
    QColor color = QtPropertyBrowserUtils::variant2color(value_);
    return QtPropertyBrowserUtils::brushValueIcon(QBrush(color));
}

/********************************************************************/
QtDynamicListProperty::QtDynamicListProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
    , length_(0)
{
    propLength_ = factory_->createProperty(QtPropertyType::INT);
    propLength_->setName("length");
    propLength_->setTitle(tr("Length"));
    addChild(propLength_);
    connect(propLength_, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotLengthChange(QtProperty*)));
}

QtDynamicListProperty::~QtDynamicListProperty()
{
}

void QtDynamicListProperty::setValue(const QVariant &value)
{
    if(value_ == value)
    {
        return;
    }

    QVariantList valueList = value.toList();
    setLength(valueList.size());

    for(int i = 0; i < items_.size(); ++i)
    {
        valueList_[i] = valueList[i];
        items_[i]->setValue(valueList[i]);
    }

    value_ = valueList_;
    emit signalValueChange(this);
}

QString QtDynamicListProperty::getValueString() const
{
    QString ret = "[";
    foreach(QtProperty *item, items_)
    {
        ret += item->getValueString();
        ret += ",";
    }
    ret += "]";
    return ret;
}

void QtDynamicListProperty::slotItemValueChange(QtProperty *item)
{
    int i = items_.indexOf(item);
    if(valueList_[i] != item->getValue())
    {
        valueList_[i] = item->getValue();
        value_ = valueList_;

        emit signalValueChange(this);
    }
}

void QtDynamicListProperty::slotItemMoveUp(QtProperty *item)
{
    int i = items_.indexOf(item);
    if(i <= 0)
    {
        return;
    }
    QtProperty *prev = items_[i - 1];
    QVariant value = prev->getValue();
    prev->setValue(item->getValue());
    item->setValue(value);
}

void QtDynamicListProperty::slotItemMoveDown(QtProperty *item)
{
    int i = items_.indexOf(item);
    if(i + 1 >= items_.size())
    {
        return;
    }
    QtProperty *next = items_[i + 1];
    QVariant value = next->getValue();
    next->setValue(item->getValue());
    item->setValue(value);
}

void QtDynamicListProperty::slotItemDelete(QtProperty *item)
{
    int i = items_.indexOf(item);
    for(; i < items_.size() - 1; ++i)
    {
        items_[i]->setValue(items_[i + 1]->getValue());
    }

    setLength(length_ - 1);
}

void QtDynamicListProperty::slotLengthChange(QtProperty *property)
{
    int length = property->getValue().toInt();
    setLength(length);

    emit signalValueChange(this);
}

void QtDynamicListProperty::setLength(int length)
{
    length = std::max(0, length);
    if(length_ == length)
    {
        return;
    }

    while(length_ < length)
    {
        appendItem();
        ++length_;
    }
    while(length_ > length)
    {
        popItem();
        --length_;
    }
    propLength_->setValue(length_);
    value_ = valueList_;
}

QtProperty* QtDynamicListProperty::appendItem()
{
    QtDynamicItemProperty *prop = dynamic_cast<QtDynamicItemProperty*>(factory_->createProperty(QtPropertyType::DYNAMIC_ITEM));
    prop->setName(QString::number(items_.size()));
    prop->setValueType(getAttribute("valueType").toString());

    QVariant valueDefault = getAttribute("valueDefault");
    prop->setValue(valueDefault);

    QVariantMap attr = getAttribute("valueAttributes").toMap();
    for(QVariantMap::iterator it = attr.begin(); it != attr.end(); ++it)
    {
       prop->getImpl()->setAttribute(it.key(), it.value());
    }

    connect(prop, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(slotItemValueChange(QtProperty*)));
    connect(prop, SIGNAL(signalMoveUp(QtProperty*)), this, SLOT(slotItemMoveUp(QtProperty*)));
    connect(prop, SIGNAL(signalMoveDown(QtProperty*)), this, SLOT(slotItemMoveDown(QtProperty*)));
    connect(prop, SIGNAL(signalDelete(QtProperty*)), this, SLOT(slotItemDelete(QtProperty*)));

    valueList_.append(valueDefault);
    items_.append(prop);

    this->addChild(prop);
    return prop;
}

void QtDynamicListProperty::popItem()
{
    QtProperty *item = items_.back();
    items_.pop_back();
    valueList_.pop_back();

    removeChild(item);
    delete item;
}

/********************************************************************/
QtDynamicItemProperty::QtDynamicItemProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
    , impl_(NULL)
{

}

QtDynamicItemProperty::~QtDynamicItemProperty()
{
    if(impl_ != NULL)
    {
        delete impl_;
    }
}

void QtDynamicItemProperty::setValueType(Type type)
{
    if(impl_ != NULL)
    {
        if(impl_->getType() == type)
        {
            return;
        }
        delete impl_;
    }
    impl_ = factory_->createProperty(type);
    assert(impl_ != NULL);

    connect(impl_, SIGNAL(signalValueChange(QtProperty*)), this, SLOT(onImplValueChange(QtProperty*)));
}

void QtDynamicItemProperty::setValue(const QVariant &value)
{
    impl_->setValue(value);
}

void QtDynamicItemProperty::onImplValueChange(QtProperty *property)
{
    emit signalValueChange(this);
}

/********************************************************************/
QtFloatListProperty::QtFloatListProperty(Type type, QtPropertyFactory *factory)
    : QtProperty(type, factory)
{

}

QtFloatListProperty::~QtFloatListProperty()
{

}

QString QtFloatListProperty::getValueString() const
{
    int size = getAttribute(QtAttributeName::Size).toInt();

    QString ret;
    ret += "[";

    QVariantList lst = value_.toList();
    for(int i = 0; i < size; ++i)
    {
        if(i != 0)
        {
            ret += ", ";
        }

        double v = 0.0;
        if(i < lst.size())
        {
            v = lst[i].toDouble();
        }
        ret += QString::number(v);
    }

    ret += "]";
    return ret;
}
