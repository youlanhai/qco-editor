#include "resource_content.h"
#include "qresourcetable.h"
#include "project_mgr.h"
#include "framework.h"
#include "resource_mgr.h"
#include "importer.h"
#include "icon_cache.h"

#include "tools/log_tool.h"
#include "tools/qstring_tool.h"
#include "tools/file_tools.h"

#include <QDir>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QMessageBox>
#include <QMenu>
#include <QFileDialog>

#include <QLineEdit>

#include <cmath>

namespace Editor
{
    static QStringList listDir(QDir &dir, const QString & parentPath, int filter)
    {
        QStringList ret;

        QStringList files = dir.entryList((QDir::Filter)filter);
        foreach(const QString &path, files)
        {
            QString relativePath = joinPath(parentPath, path);
            if( Framework::instance()->resource_->existFile(relativePath))
            {
                ret << relativePath;
            }
        }
        
        return ret;
    }

    class ContentTableItemDelegate : public QStyledItemDelegate
    {
        QSize   iconSize_;
        int     textHeight_;
    public:
        explicit ContentTableItemDelegate(const QSize &iconSize, QObject *parent = 0)
            : QStyledItemDelegate(parent)
            , iconSize_(iconSize)
            , textHeight_(30)
        {}

        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            QRect rect = option.rect;
            rect.setTop(rect.bottom() - textHeight_);
            rect.adjust(10, 0, -10, 0);
            editor->setGeometry(rect);
        }

        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
        {
            painter->save();

            QStyleOptionViewItem myOption = option;
            myOption.decorationAlignment = Qt::AlignCenter;
            myOption.displayAlignment = Qt::AlignHCenter | Qt::AlignTop;

            //填充背景
            painter->fillRect(myOption.rect, option.backgroundBrush);
//            if(myOption.state & QStyle::State_Selected)
//            {
//                //选中颜色加深
//                int ds = 4;
//                QRect rect = myOption.rect.adjusted(ds, ds, -ds, -ds);
//                painter->fillRect(rect, QColor(180, 180, 180));
//            }

            // draw icon
            QVariant value = index.data(Qt::DecorationRole);
            if(value.canConvert<QIcon>())
            {
                QIcon icon = value.value<QIcon>();

                QIcon::Mode mode = QIcon::Normal;
                if(myOption.state & QStyle::State_Selected)
                {
                    mode = QIcon::Selected;
                }
                else if(myOption.state & QStyle::State_Active)
                {
                    mode = QIcon::Active;
                }
                else if(!(myOption.state & QStyle::State_Enabled))
                {
                    mode = QIcon::Disabled;
                }

                QIcon::State state = (myOption.state & QStyle::State_On) ? QIcon::On : QIcon::Off;
                QPixmap pixmap = icon.pixmap(iconSize_, mode, state);

                QRect pixRect = pixmap.rect();
                QRect rect = myOption.rect;
                rect.setHeight(rect.height() - textHeight_);

                const int x = rect.center().x() - pixRect.width() / 2;
                const int y = rect.center().y() - pixRect.height() / 2;

                //画一个背景框
                int ds = 2;
                QRect bgRect(x, y, pixRect.width(), pixRect.height());
                bgRect.adjust(-ds, -ds, ds, ds);

                painter->setPen(QPen(QColor(204, 204, 204)));
                painter->drawRect(bgRect);

                //画图标
                painter->drawPixmap(QRect(x, y, pixmap.rect().width(), pixmap.rect().height()), pixmap);
            }

            // draw text
            QString text = displayText(index.data(Qt::DisplayRole), QLocale::system());
            QRect rect = myOption.rect;
            rect.setTop(rect.bottom() - textHeight_);
            rect.adjust(10, 0, -10, 0);

            QTextOption textOption;
            textOption.setAlignment(myOption.displayAlignment);
            textOption.setWrapMode(QTextOption::WrapAnywhere);

            QColor color = index.data(Qt::ForegroundRole).value<QColor>();
            painter->setPen(QPen(color));
            painter->drawText(rect, text, textOption);

            painter->restore();
        }
    };

    ResourceContent::ResourceContent(QObject *parent, QResourceTable *tableView, QWidget *widget)
        : QObject(parent)
        , tableView_(tableView)
        , widget_(widget)
        , iconSize_(64, 64)
        , iconCache_(nullptr)
    {
        tableView_->setItemDelegate(new ContentTableItemDelegate(iconSize_, this));
        tableView_->setColumnCount(5);
        tableView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
        tableView_->setSelectionBehavior(QAbstractItemView::SelectItems);

        connect(tableView_, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(onItemDoubleClicked(QTableWidgetItem*)));
        connect(tableView_, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onItemChanged(QTableWidgetItem*)));
        connect(tableView_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));

        contextMenu_ = new QMenu(tableView_);
        contextMenu_->addAction(tr("refresh"), this, SLOT(onActionRefresh()));
        contextMenu_->addAction(tr("create folder"), this, SLOT(onActionCreateFolder()));
        contextMenu_->addAction(tr("import"), this, SLOT(onActionImport()));
        contextMenu_->addAction(tr("export"), this, SLOT(onActionExport()));
        actionRename = contextMenu_->addAction(tr("rename"), this, SLOT(onActionRename()));
        actionRemove = contextMenu_->addAction(tr("remove"), this, SLOT(onActionRemove()));

        iconCache_ = new IconCache(this);
        connect(iconCache_, SIGNAL(signalIconLoaded(QString,QIcon)), this, SLOT(slotIconLoaded(QString,QIcon)));
    }

    bool ResourceContent::openDirectory(const QString &path)
    {
        assetPath_ = path;
        tableView_->setAssetPath(assetPath_);

        currentPath_ = "empty";
        slotShowDirectory("");
        return true;
    }

    void ResourceContent::slotShowDirectory(const QString &path)
    {
        if(currentPath_ == path)
        {
            return;
        }

        //仅清除任务，不清除缓存，可能会导致内存占用过高。
        iconCache_->clearTasks();

        currentPath_ = path;
        widget_->setWindowTitle(QString(tr("Resource Content:/%1")).arg(currentPath_));
        path2item_.clear();
        blankItems_.clear();
        tableView_->clear();
        tableView_->setCurrentPath(currentPath_);

        QString fullPath = joinPath(assetPath_, currentPath_);
        QDir dir(fullPath);
        if(!dir.exists())
        {
            LOG_ERROR("The path '%s' doesn't exist.", fullPath.toUtf8().data());
            return;
        }

        files_ = listDir(dir, currentPath_, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        foreach(const QString & path, files_)
        {
            loadItem(path);
        }

        doLayout();
    }

    void ResourceContent::doLayout()
    {
        for(int r = 0; r < tableView_->rowCount(); ++r)
        {
            for(int c = 0; c < tableView_->columnCount(); ++c)
            {
                tableView_->takeItem(r, c);
            }
        }

        int textHeight = 30;
        QSize borderSize(20, 10);
        QSize gridSize = iconSize_ + borderSize * 2;

        const int cols = std::max(1, std::min(path2item_.size(), tableView_->width() / gridSize.width()));
        const int rows = (int)ceil(float(path2item_.size()) / float(cols));
        LOG_DEBUG("Resource Content rows = %d, cols = %d", rows, cols);

        tableView_->setColumnCount(cols);
        tableView_->setRowCount(rows);

        for(int i = 0; i < rows; ++i)
        {
            tableView_->setRowHeight(i, gridSize.height() + textHeight);
        }
        for(int i = 0; i < cols; ++i)
        {
            tableView_->setColumnWidth(i, gridSize.width());
        }
        tableView_->setIconSize(iconSize_);


        int i = 0;
        foreach(const QString &path, files_)
        {
            QTableWidgetItem *item = path2item_.value(path);
            int r = i / cols;
            int c = i % cols;
            tableView_->setItem(r, c, item);
            ++i;
        }

        const int blankOffset = i;
        const int numGrids = cols * rows;
        for(; i < numGrids; ++i)
        {
            int blankIndex = i - blankOffset;
            if(blankIndex >= blankItems_.size())
            {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setFlags(Qt::NoItemFlags);
                blankItems_.append(item);
            }
            tableView_->setItem(i / cols, i % cols, blankItems_[blankIndex]);
        }
    }

    QTableWidgetItem* ResourceContent::loadItem(const QString &filePath)
    {
        QTableWidgetItem *item = new QTableWidgetItem();

        QString fileName = getFileName(filePath);
        item->setText(fileName);

        QString fullPath = joinPath(assetPath_, filePath);

        QIcon icon;
        if(QFileInfo(fullPath).isDir())
        {
            icon = QFileIconProvider().icon(QFileIconProvider::Folder);
        }
        else
        {
            QString ext = getFileExt(fileName);
            if(!ext.isEmpty())
            {
                if(ext == ".png" || ext == ".jpg")
                {
                    icon = iconCache_->getFromCache(filePath, true);
                }
                else
                {
                    ext.remove(0, 1); // remove the '.'
                    QString iconPath = QString(":/icon/%1.png").arg(ext);

                    if(QFile::exists(iconPath))
                    {
                        icon.addFile(iconPath);
                    }
                }
            }

            if(icon.isNull())
            {
                icon = QFileIconProvider().icon(QFileIconProvider::File);
            }
        }
        item->setIcon(icon);
        item->setTextAlignment(Qt::AlignBottom | Qt::AlignHCenter);
        item->setBackgroundColor(QColor(255, 255, 255, 255));

        tableView_->setItemPath(item, filePath);
        path2item_[filePath] = item;
        return item;
    }

    void ResourceContent::onItemDoubleClicked(QTableWidgetItem *item)
    {
        if(!Framework::instance()->checkCurrentLayoutModified())
        {
            return;
        }

        QString path = tableView_->getItemPath(item);
        QString fullPath = tableView_->getItemFullPath(item);
        LOG_DEBUG("item clicked: %s", path.toUtf8().data());

        if(getFileExt(path) == ".layout")
        {
            Framework::instance()->lockGL();
            bool ret = Framework::instance()->loadLayout(path);
            Framework::instance()->unlockGL();
            
            if(!ret)
            {
                QMessageBox::critical(nullptr, tr("Error"), tr("Failed to open layout '%1'").arg(path));
            }
        }
        else if(QFileInfo(fullPath).isDir())
        {
            slotShowDirectory(path);
        }
    }

    void ResourceContent::onItemChanged(QTableWidgetItem *item)
    {
        QString newName = item->text();
        QString oldPath = tableView_->getItemPath(item);
        if(newName != getFileName(oldPath))
        {
            QString newPath = joinPath(currentPath_, newName);
            if(!Framework::instance()->resource_->renameFile(oldPath, newPath))
            {
                // roll back
                blockSignals(true);
                item->setText(getFileName(oldPath));
                blockSignals(false);
            }
            else
            {
                QTableWidgetItem *item = path2item_.value(newPath);
                if(item != nullptr)
                {
                    tableView_->setCurrentItem(item);
                }
            }
        }
    }

    void ResourceContent::onContextMenuRequested(const QPoint &pos)
    {
        QTableWidgetItem *item = tableView_->currentItem();
        bool existSelection = (item != nullptr && !tableView_->getItemPath(item).isEmpty());
        actionRemove->setEnabled(existSelection);
        actionRename->setEnabled(existSelection);

        contextMenu_->exec(tableView_->mapToGlobal(pos));
    }

    void ResourceContent::onActionRefresh()
    {
        QString path = currentPath_;
        currentPath_ = "--empty";
        slotShowDirectory(path);
    }

    void ResourceContent::onActionRename()
    {
        if(tableView_->currentItem() != nullptr)
        {
            tableView_->editItem(tableView_->currentItem());
        }
    }

    void ResourceContent::onActionRemove()
    {
        QList<QTableWidgetItem*> items = tableView_->selectedItems();
        if(items.empty())
        {
            return;
        }

        QStringList paths;
        foreach(QTableWidgetItem* item, items)
        {
            paths << tableView_->getItemPath(item);
        }

        QString info = QString(tr("Are you sure to remove file:\n%1")).arg(paths.join('\n'));
        if(QMessageBox::Yes != QMessageBox::question(nullptr, tr("Ensure"), info))
        {
            return;
        }

        foreach(QString path, paths)
        {
            Framework::instance()->resource_->removeFile(path);
        }
    }

    void ResourceContent::onActionCreateFolder()
    {
        QString parentPath = currentPath_;
        QTableWidgetItem *item = tableView_->currentItem();
        if(item != nullptr)
        {
            parentPath = tableView_->getItemPath(item);
            if(QFileInfo(joinPath(assetPath_, parentPath)).isFile())
            {
                parentPath = getFilePath(parentPath);
            }
        }

        QString folderPath;
        if(Framework::instance()->resource_->createNewFolder(folderPath, parentPath))
        {
            slotShowDirectory(parentPath);

            QTableWidgetItem *item = path2item_.value(folderPath);
            if(item != nullptr)
            {
                tableView_->setCurrentItem(item);
                tableView_->editItem(item);
            }
        }
    }

    void ResourceContent::onActionExport()
    {
        if(!Framework::instance()->checkCurrentLayoutModified())
        {
            return;
        }

        QStringList paths;
        QList<QTableWidgetItem*> items = tableView_->selectedItems();
        foreach(QTableWidgetItem *item, items)
        {
            paths << tableView_->getItemPath(item);
        }

        if(paths.empty())
        {
            paths << currentPath_;
        }

        if(Framework::instance()->importer_->exportAssets(paths))
        {
            QMessageBox::information(nullptr, tr("Info"), tr("Export succes."));
        }
        else
        {
            QMessageBox::critical(nullptr, tr("Error"), tr("Export Failed."));
        }
    }

    void ResourceContent::onActionImport()
    {
        if(!Framework::instance()->checkCurrentLayoutModified())
        {
            return;
        }

        QString dstPath = currentPath_;
        QTableWidgetItem *item = tableView_->currentItem();
        if(item != nullptr)
        {
            dstPath = tableView_->getItemPath(item);
            if(QFileInfo(tableView_->getItemFullPath(item)).isFile())
            {
                dstPath = getFilePath(dstPath);
            }
        }

        QFileDialog dialog;
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        if(dialog.exec() == QFileDialog::Accepted)
        {
            QStringList files = dialog.selectedFiles();
            Framework::instance()->importer_->importAssets(files, dstPath);
        }
    }

    void ResourceContent::slotAddFile(const QString &path, bool refreshView)
    {
        if(getFilePath(path) != currentPath_)
        {
            return;
        }
        
        if(path2item_.contains(path))
        {
            return;
        }

        QTableWidgetItem *item = loadItem(path);
        if(item != nullptr)
        {
            files_ << path;
            
            if(refreshView)
            {
                files_.sort(Qt::CaseInsensitive);
                doLayout();
            }
        }
    }

    void ResourceContent::slotRemoveFile(const QString &path, bool refreshView)
    {
        // current path is a sub directory of path.
        if(currentPath_.startsWith(path))
        {
            slotShowDirectory("");
            return;
        }

        auto it = path2item_.find(path);
        if(it != path2item_.end())
        {
            path2item_.erase(it);
            delete it.value();

            files_.removeOne(path);

            if(refreshView)
            {
                doLayout();
            }
        }
    }

    void ResourceContent::slotRenameFile(const QString &oldName, const QString &newName)
    {
        //只用处理文件自己就可以了。如果是目录改名，所有子文件都会发出重命名消息。
        if(currentPath_ == oldName)
        {
            slotShowDirectory(newName);
            return;
        }
        
        // 都是不相关的文件
        if(getFilePath(oldName) != currentPath_ &&
           getFilePath(newName) != currentPath_)
        {
            return;
        }
        
        slotRemoveFile(oldName, false);
        slotAddFile(newName, false);
        
        doLayout();
    }

    void ResourceContent::slotIconLoaded(const QString &path, const QIcon &icon)
    {
        QTableWidgetItem *item = path2item_.value(path);
        if(item != nullptr)
        {
            item->setIcon(icon);
        }
    }
}
