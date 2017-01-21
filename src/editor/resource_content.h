#ifndef RESOURCE_CONTENT_H
#define RESOURCE_CONTENT_H

#include "config.h"
#include <QObject>
#include <QSize>
#include <QHash>
#include <QVector>

class QResourceTable;
class QTableWidgetItem;
class QMenu;
class QAction;

namespace Editor
{
    class IconCache;

    class EDITOR_DLL ResourceContent : public QObject
    {
        Q_OBJECT
    public:
        explicit ResourceContent(QObject *parent, QResourceTable *tableView, QWidget *widget);

        bool openDirectory(const QString &path);

    signals:

    public slots:
        void slotShowDirectory(const QString &path);
        void onItemDoubleClicked(QTableWidgetItem *item);
        void onItemChanged(QTableWidgetItem *item);

        void onContextMenuRequested(const QPoint &pos);
        void onActionRefresh();
        void onActionRename();
        void onActionRemove();
        void onActionCreateFolder();
        void onActionExport();
        void onActionImport();

        void slotAddFile(const QString &path, bool refreshView = true);
        void slotRemoveFile(const QString &path, bool refreshView = true);
        void slotRenameFile(const QString &oldName, const QString &newName);

        void slotIconLoaded(const QString &path, const QIcon &icon);

    private:
        QTableWidgetItem* loadItem(const QString &filePath);
        void doLayout();

        QResourceTable*     tableView_;
        QWidget*            widget_;
        QMenu*              contextMenu_;
        IconCache*          iconCache_;

        QString             assetPath_;
        QString             currentPath_;
        QSize               iconSize_;

        QHash<QString, QTableWidgetItem*>  path2item_;
        QStringList         files_;
        QVector<QTableWidgetItem*>  blankItems_;

        QAction*    actionRemove;
        QAction*    actionRename;
    };
}

#endif // RESOURCE_CONTENT_H
