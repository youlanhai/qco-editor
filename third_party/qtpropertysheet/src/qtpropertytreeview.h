#ifndef QTPROPERTYTREEVIEW_H
#define QTPROPERTYTREEVIEW_H

#include "qtpropertyconfig.h"
#include <QTreeWidget>

class QtTreePropertyBrowser;

class QTPROPERTYSHEET_DLL QtPropertyTreeView : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QtPropertyTreeView(QWidget *parent = 0);

    void setEditorPrivate(QtTreePropertyBrowser *editorPrivate)
    {
        editorPrivate_ = editorPrivate;
    }

    QTreeWidgetItem* indexToItem(const QModelIndex &index)
    {
        return itemFromIndex(index);
    }

protected:
    void keyPressEvent(QKeyEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QtTreePropertyBrowser *editorPrivate_;
};

#endif // QTPROPERTYTREEVIEW_H
