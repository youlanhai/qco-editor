#include "qresourcetree.h"

#include <QDropEvent>
#include <QFileInfo>
#include <QDir>
#include <QMimeData>
#include <QMessageBox>
#include <QToolTip>
#include <QHelpEvent>

#include "tools/log_tool.h"
#include "tools/qstring_tool.h"

#include "framework.h"
#include "resource_mgr.h"
#include "resource_mime.h"
#include "importer.h"

//DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "Resource");

QResourceTree::QResourceTree(QWidget *parent)
    : QTreeWidget(parent)
{

}

void QResourceTree::dropEvent(QDropEvent * event)
{
    if(!processDropEvent(event))
    {
        event->ignore();
    }
}

/** 拖拽需要考虑如下几种情况
  * 1. 内部移动。重命名
  * 2. 从Finder拖入assets目录之外的文件。复制文件到assets，并添加到Resource管理。
  * 3. 从Finder拖入assets目录内的文件。重命名。
  * 4. 从Finder拖入assets目录内但是不受工程管理的文件。直接添加到Resource管理。
  * 5. 从ResourceContent拖入。如果src是dst或dst的父，则操作失败。否则，为重命名。
  */
bool QResourceTree::processDropEvent(QDropEvent * event)
{
    const QMimeData *mimeData = event->mimeData();

    QModelIndex index = indexAt(event->pos());
    if(!index.isValid())
    {
        return false;
    }

    DropIndicatorPosition indicator = dropIndicatorPosition();
    if(!index.parent().isValid() && indicator != OnItem)//outside the root node.
    {
        return false;
    }

    QTreeWidgetItem *destItem = itemFromIndex(index);
    if(destItem == nullptr)
    {
        return false;
    }

    Editor::ResourceMgr *resourceMgr = Editor::Framework::instance()->resource_;
    QString dstPath;

    if(indicator == OnItem)
    {
        dstPath = getItemFullPath(destItem);
        if(!QFileInfo(dstPath).isDir())
        {
            QMessageBox::critical(nullptr, tr("Error"), tr("The dest path is not a directory."));
            return false;
        }
    }
    else
    {
        destItem = destItem->parent();
        dstPath = getItemFullPath(destItem);
    }

    QStringList sourceFiles;
    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        foreach(const QUrl &url, urlList)
        {
            QString path = url.toLocalFile();
            sourceFiles.append(path);
        }
    }
    else if(mimeData->hasFormat(Editor::MIME_STRING_LIST))
    {
        Editor::parseMimeStringList(sourceFiles, mimeData->data(Editor::MIME_STRING_LIST));
    }
    else
    {
        return false;
    }

    QString dstRelative = resourceMgr->toResourcePath(dstPath);
    if(!Editor::Framework::instance()->importer_->importAssets(sourceFiles, dstRelative))
    {
        return false;
    }

    event->accept();
    return true;
}

QStringList QResourceTree::mimeTypes() const
{
    QStringList qstrList;
    qstrList.append("text/uri-list");
    qstrList.append(Editor::MIME_STRING_LIST);
    return qstrList;
}

QMimeData *QResourceTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
    QMimeData *data = new QMimeData();
    if(data != nullptr)
    {
        QStringList paths;
        foreach(const QTreeWidgetItem *item, items)
        {
            QString path = getItemFullPath(item);
            paths.append(path);
        }

        QByteArray bytes;
        Editor::writeMimeStringList(bytes, paths);
        data->setData(Editor::MIME_STRING_LIST, bytes);
    }
    return data;
}

Qt::DropActions QResourceTree::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QString QResourceTree::getItemPath(const QTreeWidgetItem *item) const
{
    return item->data(0, PathRoleIndex).toString();
}

QString QResourceTree::getItemFullPath(const QTreeWidgetItem *item) const
{
    return Editor::joinPath(assetPath_, getItemPath(item));
}

void QResourceTree::setItemPath(QTreeWidgetItem *item, const QString &path)
{
    item->setData(0, PathRoleIndex, QVariant(path));
}
