#include "qhierarchytree.h"
#include "resource_mime.h"

#include <QDropEvent>
#include <QMimeData>

QHierarchyTree::QHierarchyTree(QWidget *parent)
    : QTreeWidget(parent)
{

}

QStringList QHierarchyTree::mimeTypes() const
{
    QStringList qstrList = QTreeWidget::mimeTypes();
    qstrList.append(Editor::MIME_STRING_LIST);
    return qstrList;
}

void QHierarchyTree::dropEvent(QDropEvent * event)
{
    if(!processDropEvent(event))
    {
        event->ignore();
    }
}

bool QHierarchyTree::processDropEvent(QDropEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
    {
        return false;
    }

    const QMimeData *mimeData = event->mimeData();
    if(mimeData != nullptr && mimeData->hasFormat(Editor::MIME_STRING_LIST))
    {
        if(dropIndicatorPosition() != OnItem)
        {
            return false;
        }

        QTreeWidgetItem *item = itemFromIndex(index);

        QStringList paths;
        Editor::parseMimeStringList(paths, mimeData->data(Editor::MIME_STRING_LIST));

        event->accept();
        emit signalFileDropped(item, paths);
        return true;
    }

    if(!index.parent().isValid() && dropIndicatorPosition() != OnItem) //outside of root.
    {
        return false;
    }

    QList<QTreeWidgetItem*> items = selectedItems();

    QTreeWidget::dropEvent(event);

    emit signalItemDropped(items);
    return true;
}
