#include "qtpropertytreedelegate.h"
#include "qtproperty.h"
#include "qtpropertytreeview.h"
#include "qttreepropertybrowser.h"

#include <QPainter>
#include <QFocusEvent>
#include <QApplication>

QtPropertyTreeDelegate::QtPropertyTreeDelegate(QObject *parent)
    : QItemDelegate(parent)
    , editorPrivate_(0)
    , editedItem_(0)
    , editedWidget_(0)
    , disablePainting_(false)
{}


int QtPropertyTreeDelegate::indentation(const QModelIndex &index) const
{
    if (!editorPrivate_)
    {
        return 0;
    }

    QtPropertyTreeView *treeWidget = editorPrivate_->getTreeWidget();

    QTreeWidgetItem *item = treeWidget->indexToItem(index);
    int indent = 0;
    while (item->parent())
    {
        item = item->parent();
        ++indent;
    }
    if (treeWidget->rootIsDecorated())
    {
        ++indent;
    }
    return indent * treeWidget->indentation();
}

void QtPropertyTreeDelegate::slotEditorDestroyed(QObject *object)
{
    if (QWidget *w = qobject_cast<QWidget *>(object))
    {
        const EditorToPropertyMap::iterator it = editorToProperty_.find(w);
        if (it != editorToProperty_.end())
        {
            propertyToEditor_.remove(it.value());
            editorToProperty_.erase(it);
        }

        if (editedWidget_ == w)
        {
            editedWidget_ = 0;
            editedItem_ = 0;
        }
    }
}

void QtPropertyTreeDelegate::closeEditor(QtProperty *property)
{
    if (QWidget *w = propertyToEditor_.value(property, 0))
    {
        w->deleteLater();
    }
}

QWidget *QtPropertyTreeDelegate::createEditor(QWidget *parent,
        const QStyleOptionViewItem &, const QModelIndex &index) const
{
    if (index.column() == 1 && editorPrivate_)
    {
        QTreeWidgetItem *item = editorPrivate_->indexToItem(index);
        QtProperty *property = editorPrivate_->indexToProperty(index);

        if (property && item && (item->flags() & Qt::ItemIsEnabled))
        {
            QWidget *editor = editorPrivate_->createEditor(property, parent);
            if (editor)
            {
                editor->setAutoFillBackground(true);
                editor->installEventFilter(const_cast<QtPropertyTreeDelegate *>(this));
                connect(editor, SIGNAL(destroyed(QObject *)), this, SLOT(slotEditorDestroyed(QObject *)));

                propertyToEditor_[property] = editor;
                editorToProperty_[editor] = property;
                editedItem_ = item;
                editedWidget_ = editor;
            }
            return editor;
        }
    }
    return 0;
}

void QtPropertyTreeDelegate::updateEditorGeometry(QWidget *editor,
        const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index)
    editor->setGeometry(option.rect.adjusted(0, 0, 0, -1));
}

void QtPropertyTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    QtProperty *property = editorPrivate_->indexToProperty(index);

    bool hasValue = false;
    if (editorPrivate_)
    {
        if(property)
        {
            hasValue = property->hasValue();
        }
    }

    QStyleOptionViewItem opt = option;
    if ((editorPrivate_ && index.column() == 0) || !hasValue)
    {
        if (property && property->isModified())
        {
            opt.font.setBold(true);
            opt.fontMetrics = QFontMetrics(opt.font);
        }
    }

    if (!hasValue && editorPrivate_->markPropertiesWithoutValue())
    {
        opt.palette.setColor(QPalette::Text, opt.palette.color(QPalette::BrightText));
    }

    opt.state &= ~QStyle::State_HasFocus;
    if (index.column() == 1)
    {
        QTreeWidgetItem *item = editorPrivate_->indexToItem(index);
        if (editedItem_ && editedItem_ == item)
            disablePainting_ = true;
    }

    QItemDelegate::paint(painter, opt, index);

    if (option.type)
    {
        disablePainting_ = false;
    }

    opt.palette.setCurrentColorGroup(QPalette::Active);
    QColor color = static_cast<QRgb>(QApplication::style()->styleHint(QStyle::SH_Table_GridLineColor, &opt));
    painter->save();
    painter->setPen(QPen(color));
    if (!editorPrivate_ || (!editorPrivate_->lastColumn(index.column()) && hasValue))
    {
        int right = (option.direction == Qt::LeftToRight) ? option.rect.right() : option.rect.left();
        painter->drawLine(right, option.rect.y(), right, option.rect.bottom());
    }
    painter->restore();
}

void QtPropertyTreeDelegate::drawDecoration(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QPixmap &pixmap) const
{
    if (disablePainting_)
        return;

    QItemDelegate::drawDecoration(painter, option, rect, pixmap);
}

void QtPropertyTreeDelegate::drawDisplay(QPainter *painter, const QStyleOptionViewItem &option,
            const QRect &rect, const QString &text) const
{
    if (disablePainting_)
        return;

    QItemDelegate::drawDisplay(painter, option, rect, text);
}

QSize QtPropertyTreeDelegate::sizeHint(const QStyleOptionViewItem &option,
            const QModelIndex &index) const
{
    return QItemDelegate::sizeHint(option, index) + QSize(3, 4);
}

bool QtPropertyTreeDelegate::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::FocusOut) {
        QFocusEvent *fe = static_cast<QFocusEvent *>(event);
        if (fe->reason() == Qt::ActiveWindowFocusReason)
            return false;
    }
    return QItemDelegate::eventFilter(object, event);
}
