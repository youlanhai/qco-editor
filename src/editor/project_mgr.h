#ifndef PORJECTMANAGER_H
#define PORJECTMANAGER_H

#include "config.h"
#include <QString>
#include <QStringList>
#include <json/document.h>
#include <iostream>
#include "runtime/singleton.h"

#include <platform/CCPlatformMacros.h>

class JsonHandle;

extern int MAX_HISTORY_FILE;
extern const char *HISTORY_FILE_NAME;

extern const char *PROJECT_FILE_EXT;
extern const char *USER_CONFIG_FILE_NAME;

namespace Editor
{
    enum class NewProjectMode
    {
        FromLaunch,
        FromApp
    };

    class EDITOR_DLL ProjectManager : public Singleton<ProjectManager>
    {
    public:
        ProjectManager();
        virtual ~ProjectManager();

        bool open(const QString & filename);
        void close();
        void save();
        bool newProject(const QString &projectPath, const QString &projectName);

        void saveOpenHistory(const QString & filename);

        bool isFileIgnored(const QString &path) const;

        const QString& getEditorResourcePath() const { return editorResourcePath_; }
        const QString& getTrashPath() const { return trashPath_; }
        const QString& getCachePath() const { return cachePath_; }

        const QString& getProjectName() const { return projectName_; }
        const QString& getProjectPath() const { return projectPath_; }
        const QString& getResourcePath() const { return resourcePath_; }
        const QString& getHistoryFilePath() const { return historyFilePath_; }
    private:

        QString     jsonFileName_;
        QString     editorResourcePath_;
        QString     trashPath_;
        QString     cachePath_;

        QString     projectName_;
        QString     projectPath_;
        QString     resourcePath_;
        QString     historyFilePath_;

        CC_SYNTHESIZE_PASS_BY_REF(QString, exportPath_, ExportPath)
        CC_SYNTHESIZE_PASS_BY_REF(QStringList, fileIgnoreList_, FileIgnoreList)
    };
}

#endif // PORJECTMANAGER_H
