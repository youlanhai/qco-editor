#ifndef RESOURCE_FOLDER_H
#define RESOURCE_FOLDER_H

#include "config.h"
#include <QObject>
#include <QMap>
#include <QDir>

class QResourceTree;
class QTreeWidgetItem;
class QMenu;
class QAction;

namespace Editor
{
    class EDITOR_DLL ResourceFolder : public QObject
    {
        Q_OBJECT
    public:
        ResourceFolder(QObject *parent, QResourceTree *tree);
        ~ResourceFolder();

        bool openDirectory(const QString &path);

    signals:
        void signalShowPath(const QString &path);

    private slots:
        void onItemClicked(QTreeWidgetItem *item, int column);
        void onItemChanged(QTreeWidgetItem *item, int column);

        void onContextMenuRequested(const QPoint &pos);
        void onActionRefresh();
        void onActionRename();
        void onActionRemove();
        void onActionCreateFolder();
        void onActionExport();
        void onActionImport();

        void slotAddFile(const QString &path);
        void slotRemoveFile(const QString &path);
        void slotRenameFile(const QString &oldName, const QString &newName);

    private:
        QTreeWidgetItem* loadPathItems(const QString &path);
        QTreeWidgetItem* createItem(const QString &path);

        // the path must relative to item path.
        QTreeWidgetItem* findItemByPath(QTreeWidgetItem* parentItem, const QString &path);

        void refreshItemPath(QTreeWidgetItem *item);

        QResourceTree*      treeView_;
        QMenu*              contextMenu_;
        QTreeWidgetItem*    rootItem_;

        QString             rootPath_;
        QDir                dir_;

        QAction*            actionRename_;
        QAction*            actionRemove_;
    };

}

#endif // RESOURCE_FOLDER_H
