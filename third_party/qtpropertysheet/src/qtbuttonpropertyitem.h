#ifndef QTBUTTONPROPERTYITEM_H
#define QTBUTTONPROPERTYITEM_H

#include "qtpropertyconfig.h"
#include <QObject>
#include <QList>

class QWidget;
class QLabel;
class QGridLayout;
class QToolButton;

class QtProperty;
class QtButtonPropertyItem;
class QtPropertyEditorFactory;

class QTPROPERTYSHEET_DLL QtButtonPropertyItem : public QObject
{
    Q_OBJECT
public:
    QtButtonPropertyItem();
    QtButtonPropertyItem(QtProperty *prop, QtButtonPropertyItem *parent, QtPropertyEditorFactory *editorFactory);
    virtual ~QtButtonPropertyItem();

    void update();
    void addChild(QtButtonPropertyItem *child);
    void removeChild(QtButtonPropertyItem *child);
    void removeFromParent();

    void setTitle(const QString &title);
    void setVisible(bool visible);

    QtButtonPropertyItem* parent(){ return parent_; }
    QtProperty* property(){ return property_; }

    void setLayout(QGridLayout *layout){ layout_ = layout; }

    void setExpanded(bool expand);
    bool isExpanded() const{ return bExpand_; }

protected slots:
    void onBtnExpand();
    void onBtnMenu();
    void onPropertyValueChange(QtProperty *property);

protected:
    QtProperty* property_;
    QLabel*     label_;
    QWidget*    editor_; // can be null
    QLabel*     valueLabel_;

    QToolButton* titleButton_;
    QToolButton* titleMenu_;

    QWidget*     container_;
    QGridLayout* layout_;

    QtButtonPropertyItem* parent_;
    QList<QtButtonPropertyItem*> children_;

    bool        bExpand_;
};

#endif // QTBUTTONPROPERTYITEM_H
