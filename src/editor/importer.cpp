#include "importer.h"

#include <QDir>
#include <QDirIterator>
#include <QMessageBox>

#include "tools/log_tool.h"
#include "tools/file_tools.h"
#include "tools/qstring_tool.h"

#include "project_mgr.h"
#include "framework.h"
#include "resource_mgr.h"
#include "loading_dialog.h"

#include "properties/property_group.h"

#include "importers/layout_importer.h"
#include "importers/default_importer.h"
#include "exporters/layout_exporter.h"

#include <qtproperty.h>

const char *LAYOUT_FILE_EXT = ".layout";

namespace Editor
{
    Importer::Importer(QObject *parent)
        : QObject(parent)
    {
        importers_.push_back(new LayoutImporter());
        importers_.push_back(new DefaultImporter());

        exporters_.push_back(new LayoutExporter());
        exporters_.push_back(new DefaultImporter());
    }

    Importer::~Importer()
    {
        for(FileImporter *p : importers_)
        {
            delete p;
        }

        for(FileImporter *p : exporters_)
        {
            delete p;
        }
    }

    bool Importer::exportAssets(const QString &path)
    {
        QStringList files;
        files << path;
        return exportAssets(files);
    }

    bool Importer::exportAssets(const QStringList &srcFileList)
    {
        const QString &exportPath = ProjectManager::instance()->getExportPath();
        const QString &resourcePath = ProjectManager::instance()->getResourcePath();

        QDir dirExport(exportPath);
        if(!dirExport.exists() && !dirExport.mkpath("."))
        {
            LOG_ERROR("Failed to create export dir '%s'", exportPath.toUtf8().data());
            return false;
        }

        QList<ExportTask> tasks;

        // collect files
        foreach(QString path, srcFileList)
        {
            if(!Framework::instance()->resource_->existFile(path))
            {
                LOG_ERROR("The path '%s' must be a relative path to assets.", path.toUtf8().data());
                return false;
            }

            QString dstPath = joinPath(exportPath, path);
            QString srcPath = joinPath(resourcePath, path);
            LOG_INFO("Export assets: from '%s' to '%s'", srcPath.toUtf8().data(), dstPath.toUtf8().data());

            if(QFileInfo(srcPath).isFile())
            {
                QDir parentDir(getFilePath(dstPath));
                if(!parentDir.exists() && !parentDir.mkpath("./"))
                {
                    LOG_ERROR("Failed to create folder '%s'", parentDir.path().toUtf8().data());
                    return false;
                }

                tasks.append(ExportTask(srcPath, dstPath));
            }
            else
            {
                QDir dstDir(dstPath);
                if(!dstDir.exists() && !dstDir.mkpath("./"))
                {
                    LOG_ERROR("Failed to create dir '%s'", dstDir.path().toUtf8().data());
                    return false;
                }

                QDir resourceDir(srcPath);
                QDirIterator iterator(resourceDir, QDirIterator::Subdirectories);
                while(iterator.hasNext())
                {
                    ExportTask task;

                    task.srcPath = iterator.next();
                    QString name = getFileName(task.srcPath);
                    if(name[0] == '.' || ProjectManager::instance()->isFileIgnored(name))
                    {
                        continue;
                    }

                    task.dstPath = dstDir.absoluteFilePath(resourceDir.relativeFilePath(task.srcPath));
                    if(QFileInfo(task.srcPath).isDir())
                    {
                        if(!resourceDir.exists(task.dstPath) && !resourceDir.mkdir(task.dstPath))
                        {
                            LOG_ERROR("Failed to create dir '%s'", task.dstPath.toUtf8().data());
                            return false;
                        }
                    }
                    else
                    {
                        tasks.append(task);
                    }
                }
            }
        }

        return processExportTasks(tasks);
    }

    bool Importer::processExportTasks(QList<ExportTask> tasks)
    {
        // fast export
        if(tasks.size() < 10)
        {
            foreach(const ExportTask &task, tasks)
            {
                if(!exportFile(task.srcPath, task.dstPath))
                {
                    LOG_ERROR("Failed to export file '%s'", task.srcPath.toUtf8().data());
                    return false;
                }
            }
            return true;
        }

        // show export progress
        int index = 0;
        auto func = [&index, &tasks, this](LoadingDialog *p) -> bool
        {
            int count = std::max(1, tasks.size() / 100);
            int end = std::min(index + count, tasks.size());
            for(; index < end; ++index)
            {
                const ExportTask &task = tasks[index];

                p->showMessage(task.dstPath);
                if(!this->exportFile(task.srcPath, task.dstPath))
                {
                    p->showMessage(tr("Failed to export file '%1'").arg(task.srcPath));
                    return false;
                }
            }

            p->setProgress((float)index / (float)tasks.size());
            if(index >= tasks.size())
            {
                p->finishLoading();
                p->accept();
            }
            return true;
        };

        LoadingDialog dialog(func);
        return dialog.exec() == QDialog::Accepted;
    }


    static void collectImportTasks(const QString &srcPath, const QString &dstPath, QList<ImportTask> &tasks)
    {
        tasks.push_back(ImportTask(ImportTask::COPY, srcPath, dstPath));

        QDir dir(srcPath);
        if(dir.exists())
        {
            QDirIterator iterator(dir, QDirIterator::Subdirectories);
            while(iterator.hasNext())
            {
                QString path = iterator.next();
                QString name = getFileName(path);
                if(name[0] == '.' || ProjectManager::instance()->isFileIgnored(name))
                {
                    continue;
                }

                QString relativePath = dir.relativeFilePath(path);
                tasks.push_back(ImportTask(ImportTask::COPY, path, joinPath(dstPath, relativePath)));
            }
        }
    }

    bool Importer::importAssets(const QString &srcPath, const QString &dstPath)
    {
        QStringList srcFiles;
        srcFiles.append(srcPath);

        return importAssets(srcFiles, dstPath);
    }

    bool Importer::importAssets(const QStringList &srcFiles, const QString &dstPath)
    {
        ResourceMgr *resourceMgr = Framework::instance()->resource_;
        if(!resourceMgr->existFile(dstPath))
        {
            QMessageBox::critical(nullptr, tr("Error"), tr("The dest path '%1' doens't exist.").arg(dstPath));
            return false;
        }

        QString dstFullPath = resourceMgr->toAbsolutePath(dstPath);

        QList<ImportTask> tasks;
        QString resourcePath = resourceMgr->getResourcePath();
        foreach(const QString &srcFilePath, srcFiles)
        {
            LOG_DEBUG("Try import file: %s", srcFilePath.toUtf8().data());
            if(isSubPathOf(dstFullPath, srcFilePath))
            {
                QString info = QString(tr("The source file '%1' is parent of destination path.")).arg(srcFilePath);
                QMessageBox::critical(nullptr, tr("Error"), info);
                return false;
            }

            QString dstRelativePath = joinPath(dstPath, getFileName(srcFilePath));
            if(resourceMgr->existFile(dstRelativePath))
            {
                QMessageBox::critical(nullptr, tr("Error"), tr("The dest path '%1' has been exist.").arg(dstRelativePath));
                return false;
            }

            if(isSubPathOf(srcFilePath, resourcePath))
            {
                QString srcRelativePath = resourceMgr->toResourcePath(srcFilePath);
                if(resourceMgr->existFile(srcRelativePath))
                {
                    tasks.push_back(ImportTask(ImportTask::RENAME, srcRelativePath, dstRelativePath));
                }
                else
                {
                    collectImportTasks(srcFilePath, dstRelativePath, tasks);
                }
            }
            else
            {
                collectImportTasks(srcFilePath, dstRelativePath, tasks);
            }
        }

        return processImportTasks(tasks);
    }

    bool Importer::processImportTasks(QList<ImportTask> tasks)
    {
        int index = 0;
        auto func = [&index, &tasks, this](LoadingDialog *p) -> bool
        {
            auto resourceMgr = Framework::instance()->resource_;
            int count = std::max(1, tasks.size() / 100);
            for(int i = 0; i < count && index < tasks.size(); ++i, ++index)
            {
                const ImportTask &task = tasks[index];
                p->showMessage(task.dstPath);

                if(task.type == ImportTask::COPY)
                {
                    QString dstFullPath = resourceMgr->toAbsolutePath(task.dstPath);
                    if(QFileInfo(task.srcPath).isDir())
                    {
                        QDir dstDir(dstFullPath);
                        if(!dstDir.exists() && !dstDir.mkdir(dstFullPath))
                        {
                            p->showMessage(tr("Failed to create directory '%1'").arg(dstFullPath));
                            return false;
                        }
                    }
                    else
                    {
                        if(!this->importFile(task.srcPath, dstFullPath))
                        {
                            p->showMessage(tr("Failed import file '%1'").arg(task.srcPath));
                            return false;
                        }
                    }

                    resourceMgr->loadOrAddMetaFile(dstFullPath);
                    emit resourceMgr->signalAddFile(task.dstPath);
                }
                else if(task.type == ImportTask::RENAME)
                {
                    if(!resourceMgr->renameFile(task.srcPath, task.dstPath))
                    {
                        p->showMessage(tr("Failed to rename file '%1' to '%2'").arg(task.srcPath, task.dstPath));
                        return false;
                    }
                }
            }

            p->setProgress((float)index / (float)tasks.size());
            if(index >= tasks.size())
            {
                p->finishLoading();
                p->accept();
            }
            return true;
        };

        LoadingDialog dialog(func);
        return dialog.exec() == QDialog::Accepted;
    }


    bool Importer::importFile(const QString &srcPath, const QString &dstPath)
    {
        for(FileImporter *p : importers_)
        {
            if(p->isAccepted(srcPath))
            {
                return p->execute(srcPath, dstPath);
            }
        }
        return false;
    }

    bool Importer::exportFile(const QString &srcPath, const QString &dstPath)
    {
        for(FileImporter *p : exporters_)
        {
            if(p->isAccepted(srcPath))
            {
                return p->execute(srcPath, dstPath);
            }
        }
        return false;
    }

    bool Importer::convertUUIDToPath(const QString &path)
    {
        LayoutExporter exporter;

        QString assetsPath = joinPath(ProjectManager::instance()->getResourcePath(), path);
        if(QFileInfo(assetsPath).isFile())
        {
            if(exporter.isAccepted(assetsPath))
            {
                return exporter.execute(assetsPath, assetsPath);
            }
            return true;
        }

        QDirIterator iterator(assetsPath, QDirIterator::Subdirectories);
        while(iterator.hasNext())
        {
            QString srcPath = iterator.next();
            if(exporter.isAccepted(srcPath) && !exporter.execute(srcPath, srcPath))
            {
                LOG_ERROR("Failed to convert uuid to path. file '%s'", srcPath.toUtf8().data());
                return false;
            }
        }
        return true;
    }

    bool Importer::convertPathToUUID(const QString &path)
    {
        LayoutImporter importer;

        QString assetsPath = joinPath(ProjectManager::instance()->getResourcePath(), path);
        if(QFileInfo(assetsPath).isFile())
        {
            if(importer.isAccepted(assetsPath))
            {
                return importer.execute(assetsPath, assetsPath);
            }
            return true;
        }

        QDirIterator iterator(assetsPath, QDirIterator::Subdirectories);
        while(iterator.hasNext())
        {
            QString srcPath = iterator.next();
            if(importer.isAccepted(srcPath) && !importer.execute(srcPath, srcPath))
            {
                LOG_ERROR("Failed to convert path to uuid. file '%s'", srcPath.toUtf8().data());
                return false;
            }
        }
        return true;
    }

} // end namespace Editor
