#ifndef RESOURCEMGR_H
#define RESOURCEMGR_H

#include "config.h"
#include <QObject>
#include <QDir>
#include <QHash>

namespace Editor
{
    class EDITOR_DLL ResourceMgr : public QObject
    {
        Q_OBJECT
    public:
        ResourceMgr(QObject *parent);
        ~ResourceMgr();

        // absolute path
        bool openDirectory(const QString &path);

        const QString& getResourcePath() const { return rootPath_; }

        const char* getMetaFileExt() const;

        QString toRelativePath(const QString &path) const;
        QString toAbsolutePath(const QString &path) const;

        // if the given path is not a child of resource path,
        // empty string will be returned.
        // else return a relative path for resource path.
        QString toResourcePath(const QString &path) const;

        QString toRealPath(const QString &path) const;

        QString uuid2path(const QString &uuid) const;
        QString path2uuid(const QString &path) const;

        bool existFile(const QString &path);

        /** add an existing internal file to resource manager.
         *  @param path     the relative file or directory path.
         */
        bool addFile(const QString &path);
        bool removeFile(const QString &path);
        bool renameFile(const QString &oldName, const QString &newName);
        bool createFolder(const QString &path);
        bool createNewFolder(QString &outPath, const QString &parentPath);

    signals:
        void signalAddFile(const QString &path);
        void signalRemoveFile(const QString &path);
        void signalRenameFile(const QString &oldName, const QString &newName);

    private:
        QString convertToRealPath(const QString &name) const;

        void setFileUUID(const QString &filePath, const QString &uuid);
        void renameFileUUID(const QString &oldPath, const QString &newPath);
        void removeFileUUID(const QString &path);

        // absolute path
        bool loadPathMetaFile(const QString &path);

        // absolute path
        bool loadOrAddPathMetaFile(const QString &srcFilePath);

        // absolute path
        bool loadMetaFile(const QString &path);

        // absolute path
        bool loadOrAddMetaFile(const QString &path);

        QString             rootPath_;
        QDir                dir_;

        typedef QHash<QString, QString> StringMap;
        StringMap           path2uuid_;
        StringMap           uuid2path_;

        friend class Importer;
    };

}

#endif // RESOURCEMGR_H
