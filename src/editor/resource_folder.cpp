#include "resource_folder.h"

#include "resource_mgr.h"
#include "tools/log_tool.h"
#include "tools/qstring_tool.h"
#include "tools/file_tools.h"
#include "qresourcetree.h"
#include "framework.h"
#include "project_mgr.h"
#include "importer.h"
#include <runtime/json_helper.h>

#include <QDir>
#include <QFile>
#include <QFileIconProvider>
#include <QMenu>
#include <QUuid>
#include <QMessageBox>
#include <QFileDialog>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "Resource");

#define EDITOR_ENABLE_UUID 1

namespace Editor
{
    ResourceFolder::ResourceFolder(QObject *parent, QResourceTree *tree)
        : QObject(parent)
        , treeView_(tree)
        , contextMenu_(nullptr)
        , rootItem_(nullptr)
        , actionRename_(nullptr)
        , actionRemove_(nullptr)
    {
        connect(treeView_, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(onItemClicked(QTreeWidgetItem*,int)));
        connect(treeView_, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
        connect(treeView_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));

        contextMenu_ = new QMenu(treeView_);
        contextMenu_->addAction(tr("refresh"), this, SLOT(onActionRefresh()));
        contextMenu_->addAction(tr("create folder"), this, SLOT(onActionCreateFolder()));
        contextMenu_->addAction(tr("import"), this, SLOT(onActionImport()));
        contextMenu_->addAction(tr("export"), this, SLOT(onActionExport()));

        actionRename_ = contextMenu_->addAction(tr("rename"), this, SLOT(onActionRename()));
        actionRemove_ = contextMenu_->addAction(tr("remove"), this, SLOT(onActionRemove()));
    }

    ResourceFolder::~ResourceFolder()
    {
    }

    bool ResourceFolder::openDirectory(const QString &path)
    {
        // do cleanup
        treeView_->clear();

        rootPath_ = path;
        dir_ = QDir(rootPath_);
        treeView_->setAssetPath(rootPath_);

        // create new tree.
        rootItem_ = loadPathItems("");
        if(rootItem_ != nullptr)
        {
            rootItem_->setText(0, getFileName(rootPath_));

            treeView_->addTopLevelItem(rootItem_);
            rootItem_->setExpanded(true);
        }
        return true;
    }

    QTreeWidgetItem* ResourceFolder::loadPathItems(const QString &path)
    {
        QTreeWidgetItem *item = createItem(path);

        QDir dir(joinPath(rootPath_, path));
        QStringList files = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString &name, files)
        {
            QString fileName = joinPath(path, name);
            if(!Framework::instance()->resource_->existFile(fileName))
            {
                continue;
            }

            QTreeWidgetItem *subItem = loadPathItems(fileName);
            if(subItem != nullptr)
            {
                item->addChild(subItem);
            }
        }
        return item;
    }

    QTreeWidgetItem* ResourceFolder::createItem(const QString &path)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, getFileName(path));
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        treeView_->setItemPath(item, path);

        QIcon icon = QFileIconProvider().icon(QFileIconProvider::Folder);
        item->setIcon(0, icon);

        return item;
    }

    QTreeWidgetItem* ResourceFolder::findItemByPath(QTreeWidgetItem *parentItem, const QString &path)
    {
        int split = path.indexOf('/');
        QString name;
        if(split >= 0)
        {
            name = path.left(split);
        }
        else
        {
            name = path;
        }

        QTreeWidgetItem *target = nullptr;
        for(int i = 0; i < parentItem->childCount(); ++i)
        {
            QTreeWidgetItem *child = parentItem->child(i);
            QString path = treeView_->getItemPath(child);
            if(getFileName(path) == name)
            {
                target = child;
                break;
            }
        }

        if(target != nullptr && split >= 0)
        {
            return findItemByPath(target, path.right(path.size() - (split + 1)));
        }
        return target;
    }

    void ResourceFolder::onItemClicked(QTreeWidgetItem *item, int /*column*/)
    {
        QString path = treeView_->getItemPath(item);
        emit signalShowPath(path);
    }

    void ResourceFolder::onItemChanged(QTreeWidgetItem *item, int column)
    {
        QString oldPath = treeView_->getItemPath(item);

        QString newName = item->text(column);
        QString newPath = joinPath(getFilePath(oldPath), newName);
        if(newPath == oldPath) // setData will also trigger the data changed event.
        {
            return;
        }

        LOG_DEBUG("Item changed '%s'", oldPath.toUtf8().data());
        if(!Framework::instance()->resource_->renameFile(oldPath, newPath))
        {
            // rollback the rename
            item->setText(column, getFileName(oldPath));

            QString info = QString("Failed rename file '%1' to '%2'").arg(oldPath, newPath);
            LOG_ERROR("%s", info.toUtf8().data());

            QMessageBox::critical(nullptr, tr("Error"), info, QMessageBox::StandardButton::Ok);
        }
    }

    void ResourceFolder::onContextMenuRequested(const QPoint &pos)
    {
        if(treeView_->currentItem() != nullptr)
        {
            assert(contextMenu_->actions().contains(actionRemove_));

            bool isRoot = treeView_->currentItem() == rootItem_;
            actionRemove_->setEnabled(!isRoot);
            actionRename_->setEnabled(!isRoot);

            contextMenu_->exec(treeView_->mapToGlobal(pos));
        }
    }

    void ResourceFolder::onActionRefresh()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            QString path = treeView_->getItemPath(item);
            QTreeWidgetItem *parentItem = item->parent();
            if(parentItem != nullptr)
            {
                parentItem->removeChild(item);
            }
            else
            {
                treeView_->removeItemWidget(item, 0);
                rootItem_ = nullptr;
            }

            delete item;
            item = loadPathItems(path);
            if(nullptr == item)
            {
                return;
            }

            if(parentItem != nullptr)
            {
                parentItem->addChild(item);
            }
            else
            {
                rootItem_ = item;
                rootItem_->setText(0, getFileName(rootPath_));
                treeView_->addTopLevelItem(item);
            }

            item->setExpanded(true);
            treeView_->setCurrentItem(item);
        }
    }

    void ResourceFolder::onActionRename()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            treeView_->editItem(item);
        }
    }

    void ResourceFolder::onActionRemove()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            QString path = treeView_->getItemPath(item);

            QString text = QString(tr("Are you sure to remove '%1'?")).arg(path);
            if(QMessageBox::Yes != QMessageBox::question(NULL, tr(""), text))
            {
                return;
            }

            if(!Framework::instance()->resource_->removeFile(path))
            {
                QString info = QString(tr("Failed to remove '%1'")).arg(path);
                LOG_ERROR("%s", info.toUtf8().data());

                QMessageBox::critical(nullptr, tr("Error"), info);
            }
        }
    }

    void ResourceFolder::onActionCreateFolder()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            QString parentPath = treeView_->getItemPath(item);
            if(QFileInfo(joinPath(rootPath_, parentPath)).isFile())
            {
                parentPath = getFilePath(parentPath);
                item = item->parent();
            }

            QString newFolderPath;
            if(!Framework::instance()->resource_->createNewFolder(newFolderPath, parentPath))
            {
                QString info = QString(tr("Failed to create folder '%1'")).arg(newFolderPath);
                LOG_ERROR("%s", info.toUtf8().data());

                QMessageBox::critical(nullptr, tr("Error"), info);
                return;
            }

            QString name = getFileName(newFolderPath);
            QTreeWidgetItem *newItem = findItemByPath(item, name);
            if(newItem != nullptr)
            {
                treeView_->setCurrentItem(newItem);
                treeView_->editItem(newItem);
            }
        }
    }

    void ResourceFolder::onActionExport()
    {
        if(!Framework::instance()->checkCurrentLayoutModified())
        {
            return;
        }

        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            QString path = treeView_->getItemPath(item);
            if(Framework::instance()->importer_->exportAssets(path))
            {
                QMessageBox::information(nullptr, tr("Info"), tr("Export succes."));
            }
            else
            {
                QMessageBox::critical(nullptr, tr("Error"), tr("Export Failed."));
            }
        }
    }

    void ResourceFolder::onActionImport()
    {
        if(!Framework::instance()->checkCurrentLayoutModified())
        {
            return;
        }

        QString dstPath;
        QTreeWidgetItem *item = treeView_->currentItem();
        if(item != nullptr)
        {
            dstPath = treeView_->getItemPath(item);
            if(QFileInfo(treeView_->getItemFullPath(item)).isFile())
            {
                dstPath = getFilePath(dstPath);
            }
        }

        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setDirectory(dstPath);
        if(dialog.exec() == QFileDialog::Accepted)
        {
            QStringList files = dialog.selectedFiles();
            Framework::instance()->importer_->importAssets(files, dstPath);
        }
    }

    void ResourceFolder::refreshItemPath(QTreeWidgetItem *item)
    {
        if(item->parent() != nullptr)
        {
            QString parentPath = treeView_->getItemPath(item->parent());

            QString oldPath = treeView_->getItemPath(item);
            QString newPath = joinPath(parentPath, getFileName(oldPath));
            if(oldPath != newPath)
            {
                treeView_->setItemPath(item, newPath);

                LOG_DEBUG("Item name change from '%s' to '%s'", oldPath.toUtf8().data(), newPath.toUtf8().data());
            }
        }

        int nChildren = item->childCount();
        for(int i = 0; i < nChildren; ++i)
        {
            refreshItemPath(item->child(i));
        }
    }

    void ResourceFolder::slotAddFile(const QString &path)
    {
        if(!QFileInfo(dir_.absoluteFilePath(path)).isDir())
        {
            return;
        }
        

        QTreeWidgetItem *parentItem = nullptr;
        QString parentPath = getFilePath(path);

        if(parentPath.isEmpty())
        {
            parentItem = rootItem_;
        }
        else
        {
            parentItem = findItemByPath(rootItem_, parentPath);
        }
        if(parentItem == nullptr)
        {
            return;
        }

        QTreeWidgetItem *item = loadPathItems(path);
        if(item != nullptr)
        {
            parentItem->addChild(item);
        }
    }

    void ResourceFolder::slotRemoveFile(const QString &path)
    {
        QTreeWidgetItem *item = findItemByPath(rootItem_, path);
        if(item != nullptr)
        {
            delete item;
        }
    }

    void ResourceFolder::slotRenameFile(const QString &oldName, const QString &newName)
    {
        if(getFilePath(oldName) == getFilePath(newName))
        {
            QTreeWidgetItem *item = findItemByPath(rootItem_, oldName);
            if(item != nullptr)
            {
                treeView_->blockSignals(true);
                
                item->setText(0, getFileName(newName));
                treeView_->setItemPath(item, newName);
                refreshItemPath(item);
                
                treeView_->blockSignals(false);
            }
        }
        else
        {
            QTreeWidgetItem *item = findItemByPath(rootItem_, oldName);
            if(item != nullptr)
            {
                delete item;
                slotAddFile(newName);
            }
        }
    }
}
