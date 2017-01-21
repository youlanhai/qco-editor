#include "resource_mgr.h"

#include "tools/log_tool.h"
#include "tools/qstring_tool.h"
#include "tools/file_tools.h"
#include "tools/json_tools.h"

#include "project_mgr.h"
#include "framework.h"
#include "importer.h"
#include "loading_dialog.h"

#include <runtime/json_helper.h>

#include <QDir>
#include <QFile>
#include <QUuid>
#include <QMessageBox>
#include <QDirIterator>
#include <QApplication>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "Resource");

#define EDITOR_ENABLE_UUID 1

const char *META_FILE_EXT = ".meta";

namespace Editor
{
    ResourceMgr::ResourceMgr(QObject *parent)
        : QObject(parent)
    {
    }

    ResourceMgr::~ResourceMgr()
    {
    }

    const char* ResourceMgr::getMetaFileExt() const
    {
        return META_FILE_EXT;
    }

    //////////////////////////////////////////////////////////////////
    // the following api are using absolute path as param.
    //////////////////////////////////////////////////////////////////

    bool ResourceMgr::openDirectory(const QString &path)
    {
        // do cleanup
        path2uuid_.clear();
        uuid2path_.clear();

        rootPath_ = path;
        dir_ = QDir(rootPath_);
        setFileUUID("", "");

        return loadPathMetaFile(rootPath_);
    }

    bool ResourceMgr::loadPathMetaFile(const QString &path)
    {
        QStringList files;
        QDirIterator iter(path, QDirIterator::Subdirectories);
        while(iter.hasNext())
        {
            QString path = iter.next();

            QString name = getFileName(path);
            if(name[0] == '.' || ProjectManager::instance()->isFileIgnored(name))
            {
                continue;
            }

            files.append(path);
        }
        
        int index = 0;
        auto func = [&index, &files, this](LoadingDialog *p) -> bool
        {
            const int StepCount = std::max(1, files.size() / 100);
            for(int i = 0; i < StepCount && index < files.size(); ++i, ++index)
            {
                const QString &path = files[index];
                if(this->loadMetaFile(path))
                {
                    p->showMessage(toResourcePath(path));
                }
                else
                {
                    p->showMessage(tr("Failed %1").arg(toResourcePath(path)));
                    continue;
                }
            }

            p->setProgress((float)index / files.size());
            if(index >= files.size())
            {
                p->finishLoading();
                p->accept();
            }
            return true;
        };

        LoadingDialog dialog(func);
        int ret = dialog.exec();

        Framework::instance()->lockGL();
        return ret == QDialog::Accepted;
    }

    bool ResourceMgr::loadOrAddPathMetaFile(const QString &srcFilePath)
    {
        if(!loadOrAddMetaFile(srcFilePath))
        {
            return false;
        }

        QFileInfo srcFileInfo(srcFilePath);
        if (srcFileInfo.isDir())
        {
            QDir sourceDir(srcFilePath);
            QStringList fileNames = sourceDir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);
            foreach (const QString &fileName, fileNames)
            {
                const QString newSrcFilePath = joinPath(srcFilePath, fileName);

                if(ProjectManager::instance()->isFileIgnored(newSrcFilePath))
                {
                    continue;
                }

                if (!loadOrAddPathMetaFile(newSrcFilePath))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool ResourceMgr::loadMetaFile(const QString &path)
    {
        assert(getFileExt(path) != META_FILE_EXT);

        JsonValue doc;
        if(!openJsonFile(path.toStdString() + META_FILE_EXT, doc) || !doc.isDict())
        {
            return false;
        }

        QString uuid = json2qstring(doc.find("uuid"));
        if(uuid.isEmpty())
        {
            return false;
        }

        setFileUUID(toResourcePath(path), uuid);
        return true;
    }

    bool ResourceMgr::loadOrAddMetaFile(const QString &path)
    {
        assert(getFileExt(path) != META_FILE_EXT);

        QString metaFilePath = path + META_FILE_EXT;
        if(QFileInfo::exists(metaFilePath))
        {
            return loadMetaFile(path);
        }
        else
        {
            // generate a new meta file.
            QString uuid = QUuid::createUuid().toString();

            JsonValue doc;
            doc.setDict();
            doc["uuid"] = jsonFrom(uuid);

            if(!saveJsonFile(metaFilePath.toStdString(), doc))
            {
                return false;
            }

            setFileUUID(toResourcePath(path), uuid);
        }
        return true;
    }

    //////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////

    QString ResourceMgr::toRelativePath(const QString &path) const
    {
        return dir_.relativeFilePath(path);
    }

    QString ResourceMgr::toAbsolutePath(const QString &path) const
    {
        return dir_.absoluteFilePath(path);
    }

    QString ResourceMgr::toResourcePath(const QString &path) const
    {
        QString filePath = QDir::cleanPath(path);
        if(filePath == rootPath_ || !filePath.startsWith(rootPath_))
        {
            return QString();
        }

        return filePath.right(filePath.size() - (rootPath_.size() + 1));
    }

    QString ResourceMgr::toRealPath(const QString &path) const
    {
        const QChar SplitChar = '$';

        QString realPath;
        for(int i = 0; i < path.size(); )
        {
            QChar ch = path[i];
            ++i;

            if(ch == SplitChar)
            {
                if(i < path.size() && path[i] == '$')
                {
                    realPath += SplitChar;
                    ++i; // resolve '$$' to '$'. ignore the following '$'.
                }
                else
                {
                    int j = i;
                    for(; j < path.size() && path[j] != '/'; ++j)
                    {
                    }
                    QString variable = path.mid(i, j - i);
                    realPath += convertToRealPath(variable);
                    i = j;
                }
            }
            else
            {
                realPath += ch;
            }
        }

        return realPath;
    }

    QString ResourceMgr::convertToRealPath(const QString &name) const
    {
        if(name == "RESOURCE")
        {
            return rootPath_;
        }
        else if(name == "PWD")
        {
            return QDir::currentPath();
        }
        else if(name == "PROJECT")
        {
            return getFilePath(rootPath_);
        }
        else
        {
            return name;
        }
    }

    QString ResourceMgr::uuid2path(const QString &uuid) const
    {
#if EDITOR_ENABLE_UUID
        if(uuid.isEmpty() || uuid[0] != '{')
        {
            return uuid;
        }
        return uuid2path_.value(uuid);
#else
        return uuid;
#endif
    }

    QString ResourceMgr::path2uuid(const QString &path) const
    {
#if EDITOR_ENABLE_UUID
        QString littlePath = path.toLower();
        return path2uuid_.value(littlePath);
#else
        return path;
#endif
    }

    void ResourceMgr::setFileUUID(const QString &filePath, const QString &uuid)
    {
        assert(dir_.isRelativePath(filePath));

        QString littlePath = filePath.toLower();

        QString oldUUID = path2uuid_.value(littlePath);
        if(!oldUUID.isEmpty() && oldUUID != uuid)
        {
            LOG_INFO("File '%s' uuid changed.", filePath.toUtf8().data());
        }

        uuid2path_[uuid] = filePath;
        path2uuid_[littlePath] = uuid;
    }

    void ResourceMgr::renameFileUUID(const QString &oldPath, const QString &newPath)
    {
        assert(dir_.isRelativePath(oldPath));
        assert(dir_.isRelativePath(newPath));

        auto it = path2uuid_.find(oldPath.toLower());
        if(it == path2uuid_.end())
        {
            LOG_ERROR("The file '%s' was not found in resouce manager.", oldPath.toUtf8().data());
            return;
        }

        QString uuid = it.value();
        path2uuid_.erase(it);

        path2uuid_[newPath.toLower()] = uuid;
        uuid2path_[uuid] = newPath;

    }

    void ResourceMgr::removeFileUUID(const QString &path)
    {
        assert(dir_.isRelativePath(path));

        auto it = path2uuid_.find(path.toLower());
        if(it == path2uuid_.end())
        {
            LOG_WARN("The file '%s' was not found in resource manager.", path.toUtf8().data());
            return;
        }

        uuid2path_.remove(it.value());
        path2uuid_.erase(it);
    }

    bool ResourceMgr::existFile(const QString &path)
    {
        return path2uuid_.contains(path.toLower());
    }

    bool ResourceMgr::addFile(const QString &path)
    {
        assert(dir_.isRelativePath(path));

        if(!dir_.exists(path))
        {
            LOG_ERROR("ResourceMgr::addFile: the file doesn't exist. '%s'", path.toUtf8().data());
            return false;
        }

        if(!ProjectManager::instance()->isFileIgnored(path))
        {
            Framework::instance()->importer_->convertPathToUUID(path);

            if(!path2uuid_.contains(path))
            {
                if(!loadOrAddPathMetaFile(toAbsolutePath(path)))
                {
                    return false;
                }

                emit signalAddFile(path);
            }
        }
        return true;
    }

    bool ResourceMgr::removeFile(const QString &path)
    {
        assert(dir_.isRelativePath(path));

        QString fullPath = toAbsolutePath(path);
        QFileInfo fileInfo(fullPath);
        if(!fileInfo.exists())
        {
            LOG_ERROR("ResourceMgr::removeFile: the file doesn't exist. '%s'", path.toUtf8().data());
            return false;
        }
        bool isDir = fileInfo.isDir();

        QString trashPath = ProjectManager::instance()->getTrashPath();
        if(!QFileInfo::exists(trashPath))
        {
            dir_.mkdir(trashPath);
        }

        QString uuid = QUuid::createUuid().toString();

        QString dstPath = joinPath(trashPath, getFileName(path) + uuid);
        if(!QFile::rename(fullPath, dstPath))
        {
            return false;
        }
        QFile::rename(fullPath + META_FILE_EXT, dstPath + META_FILE_EXT);

        if(isDir)
        {
            QList<QString> names = path2uuid_.keys();
            foreach(const QString &name, names)
            {
                if(name.startsWith(path))
                {
                    removeFileUUID(name);
                    emit signalRemoveFile(path);
                }
            }
        }
        else
        {
            removeFileUUID(path);
            emit signalRemoveFile(path);
        }
        return true;
    }

    bool ResourceMgr::renameFile(const QString &oldName, const QString &newName)
    {
        assert(dir_.isRelativePath(oldName));
        assert(dir_.isRelativePath(newName));

        if(!dir_.exists(oldName))
        {
            LOG_ERROR("ResourceMgr::renameFile: the file doesn't exist. '%s'", oldName.toUtf8().data());
            return false;
        }
        if(dir_.exists(newName))
        {
            LOG_ERROR("ResourceMgr::renameFile: the new name has been exist. '%s'", newName.toUtf8().data());
            return false;
        }

        if(!dir_.rename(oldName, newName))
        {
            LOG_ERROR("ResourceMgr::renameFile: rename '%s' to '%s' failed.", oldName.toUtf8().data(), newName.toUtf8().data());
            return false;
        }
        dir_.rename(oldName + META_FILE_EXT, newName + META_FILE_EXT);

        if(QFileInfo(toAbsolutePath(newName)).isDir())
        {
            QString lowerName = oldName.toLower();

            QList<QString> keys = path2uuid_.keys();
            foreach(const QString &name, keys)
            {
                if(name.startsWith(lowerName))
                {
                    QString name2 = newName + name.right(name.size() - oldName.size());
                    renameFileUUID(name, name2);
                    emit signalRenameFile(name, name2);
                }
            }
        }
        else
        {
            renameFileUUID(oldName, newName);
            emit signalRenameFile(oldName, newName);
        }

        return true;
    }

    bool ResourceMgr::createFolder(const QString &path)
    {
        if(!dir_.mkdir(path))
        {
            LOG_ERROR("ResourceMgr::createFolder: create folder failed. '%s'", path.toUtf8().data());
            return false;
        }
        return addFile(path);
    }

    bool ResourceMgr::createNewFolder(QString &outPath, const QString &parentPath)
    {
        QDir dirInfo(toAbsolutePath(parentPath));

        QString newFolderName = "new_folder";
        while(dirInfo.exists(newFolderName))
        {
            newFolderName += "_";
        }

        outPath = joinPath(parentPath, newFolderName);
        return createFolder(outPath);
    }
}
