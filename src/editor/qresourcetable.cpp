#include "qresourcetable.h"

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

static const int PathRoleIndex = Qt::UserRole + 1;

QResourceTable::QResourceTable(QWidget *parent)
    : QTableWidget(parent)
{

}


void QResourceTable::dropEvent(QDropEvent * event)
{
    if(!processDropEvent(event))
    {
        event->ignore();
    }
}

bool QResourceTable::processDropEvent(QDropEvent * event)
{
    const QMimeData *mimeData = event->mimeData();
    if(!mimeData->hasUrls() && !mimeData->hasFormat(Editor::MIME_STRING_LIST))
    {
        return false;
    }

    QModelIndex index = indexAt(event->pos());
    DropIndicatorPosition indicator = dropIndicatorPosition();

    QString dstPath = Editor::joinPath(assetPath_, currentPath_);
    if(index.isValid() && indicator == OnItem)
    {
        QTableWidgetItem *destItem = itemFromIndex(index);
        if(destItem == currentItem())
        {
            return false;
        }

        if(destItem != nullptr)
        {
            QString path = getItemFullPath(destItem);
            if(QFileInfo(path).isDir())
            {
                dstPath = path;
            }
        }
    }

    Editor::ResourceMgr *resourceMgr = Editor::Framework::instance()->resource_;

    QStringList sourceFiles;
    if(mimeData->hasUrls())
    {
        QList<QUrl> urlList = mimeData->urls();
        foreach(QUrl url, urlList)
        {
            sourceFiles.append(url.toLocalFile());
        }
    }
    else if(mimeData->hasFormat(Editor::MIME_STRING_LIST))
    {
        Editor::parseMimeStringList(sourceFiles, mimeData->data(Editor::MIME_STRING_LIST));
    }

    QString dstRelative = resourceMgr->toResourcePath(dstPath);
    if(!Editor::Framework::instance()->importer_->importAssets(sourceFiles, dstRelative))
    {
        return false;
    }

    event->accept();
    return true;
}

QStringList QResourceTable::mimeTypes() const
{
    QStringList qstrList;
    qstrList.append("text/uri-list");
    qstrList.append(Editor::MIME_STRING_LIST);
    return qstrList;
}

QMimeData *QResourceTable::mimeData(const QList<QTableWidgetItem *> items) const
{
    QMimeData *data = new QMimeData();
    if(data != nullptr)
    {
        QStringList paths;
        foreach(const QTableWidgetItem *item, items)
        {
            QString path = getItemFullPath(item);
            paths << path;
        }

        QByteArray bytes;
        Editor::writeMimeStringList(bytes, paths);
        data->setData(Editor::MIME_STRING_LIST, bytes);
    }
    return data;
}

Qt::DropActions QResourceTable::supportedDropActions () const
{
    return Qt::CopyAction | Qt::MoveAction;
}

QString QResourceTable::getItemPath(const QTableWidgetItem *item) const
{
    return item->data(PathRoleIndex).toString();
}

QString QResourceTable::getItemFullPath(const QTableWidgetItem *item) const
{
    return Editor::joinPath(assetPath_, getItemPath(item));
}

void QResourceTable::setItemPath(QTableWidgetItem *item, const QString &path)
{
    item->setData(PathRoleIndex, QVariant(path));
    item->setToolTip(path);
}
