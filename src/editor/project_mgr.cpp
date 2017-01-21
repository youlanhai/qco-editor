#include "project_mgr.h"
#include "runtime/json_helper.h"
#include "framework.h"
#include "tools/json_tools.h"
#include "tools/log_tool.h"
#include "tools/file_tools.h"
#include "tools/qstring_tool.h"
#include "resource_mgr.h"

#include <QDir>
#include <QFileInfo>


IMPLEMENT_SINGLETON(Editor::ProjectManager);


// extern from resource_mgr.cpp
extern const char* META_FILE_EXT;

int MAX_HISTORY_FILE = 10;
const char *HISTORY_FILE_NAME = "history.json";

const char *PROJECT_FILE_EXT = ".project";
const char *USER_CONFIG_FILE_NAME = "userconfig.json";

namespace
{
const char *ASSET_PATH = "assets";
const char *RES_PATH = "res";
const char *EXPORT_PATH = "export";
const char *TRASH_PATH = ".trash";
const char *CACHE_PATH = ".cache";
}

namespace Editor
{
    ProjectManager::ProjectManager()
    {
        std::string path = joinPath(getAppModulePath(), HISTORY_FILE_NAME);
        historyFilePath_ = QString::fromStdString(path);

        editorResourcePath_ = QString::fromStdString(joinPath(getAppResPath(), RES_PATH));
    }

    ProjectManager::~ProjectManager()
    {

    }

    bool ProjectManager::open(const QString & filename)
    {
        JsonValue document;
        if (!openJsonFile(filename.toStdString(), document) || !document.isDict())
        {
            return false;
        }
        saveOpenHistory(filename);

        jsonFileName_ = filename;
        projectPath_ = getFilePath(filename);

        QDir projectDir(projectPath_);

        JsonHandle config(document);
        projectName_ = json2qstring(config["projectName"], "project");

        resourcePath_ = projectDir.absoluteFilePath(ASSET_PATH);
        if(!QFileInfo::exists(resourcePath_))
        {
            projectDir.mkdir(resourcePath_);
        }

        trashPath_ = projectDir.absoluteFilePath(TRASH_PATH);
        if(!QFileInfo::exists(trashPath_))
        {
            projectDir.mkdir(trashPath_);
        }

        cachePath_ = projectDir.absoluteFilePath(CACHE_PATH);
        if(!QFileInfo::exists(cachePath_))
        {
            projectDir.mkdir(trashPath_);
        }

        exportPath_ = projectDir.absoluteFilePath(EXPORT_PATH);

        JsonHandle setting = config["setting"];

        fileIgnoreList_.clear();
        JsonHandle ignores = setting["ignores"];
        if(ignores.isArray())
        {
            std::string file;
            const mjson::Array &array = *ignores->rawArray();
            for(const JsonValue & value : array)
            {
                file = json2string(value);
                if(!file.empty())
                {
                    fileIgnoreList_ << QString::fromStdString(file);
                }
            }
        }
        if(!fileIgnoreList_.contains(META_FILE_EXT))
        {
            fileIgnoreList_ << META_FILE_EXT;
        }

        return true;
    }

    void ProjectManager::close()
    {

    }

    void ProjectManager::save()
    {
        JsonValue document;
        document.setDict();

        document["projectName"] = projectName_.toStdString();

        JsonValue &settings = document["setting"];
        settings.setDict();

        JsonValue &ignores = document["ignores"];
        mjson::Array *array = ignores.setArray();
        foreach(const QString &file, fileIgnoreList_)
        {
            array->append(file.toStdString());
        }

        saveJsonFile(jsonFileName_.toStdString(), document);
    }

    bool ProjectManager::newProject(const QString &projectPath, const QString &projectName)
    {
        // crate project file
        JsonValue document;
        document.setDict();

        document["projectName"] = projectName.toStdString();

        JsonValue &settings = document["setting"];
        settings.setDict();

        JsonValue &ignores = settings["ignores"];
        mjson::Array *array = ignores.setArray();
        array->append(".meta");
        array->append(".svn");
        array->append(".git");
        array->append(".DS_Store");

        QString projectFilePath = joinPath(projectPath, projectName + PROJECT_FILE_EXT);
        if(!saveJsonFile(projectFilePath.toStdString(), document))
        {
            LOG_ERROR("Faile to save project configure '%s'", projectFilePath.toUtf8().data());
            return false;
        }

        // TODO 使用导入的方式，将qco导入工程
#if 0
        // copy default assets
        QDir dir(projectPath);
        if(!dir.exists(ASSET_PATH))
        {
            dir.mkdir(ASSET_PATH);
        }

        QString destQcoPath = joinPath(QString(ASSET_PATH), QString("qco"));
        if(!dir.exists(destQcoPath))
        {
            destQcoPath = dir.absoluteFilePath(destQcoPath);

            QString resPath = QString::fromStdString(joinPath(getAppResPath(), RES_PATH));
            QString qcoPath = joinPath(resPath, "qco");

            LOG_DEBUG("Copy Asset: %s -> %s", qcoPath.toUtf8().data(), destQcoPath.toUtf8().data());
            copyRecursively(qcoPath, destQcoPath);
        }
#endif
        return open(projectFilePath);
    }

    void ProjectManager::saveOpenHistory(const QString & fileName)
    {
        JsonValue document;

        std::string historyFile = historyFilePath_.toStdString();
        if (!openJsonFile(historyFile, document) || !document.isArray())
        {
            document.setArray();
        }

        mjson::Array *array = document.rawArray();
        std::string strFileName = fileName.toStdString();
        for(auto it = array->begin(); it != array->end(); ++it)
        {
            if(it->isString() && it->rawString()->compare(strFileName) == 0)
            {
                array->erase(it);
                break;
            }
        }
        array->append(strFileName);

        while((int)array->size() > MAX_HISTORY_FILE)
        {
            array->erase(array->begin());
        }

        saveJsonFile(historyFile, document);
    }


    bool ProjectManager::isFileIgnored(const QString &path) const
    {
        if(fileIgnoreList_.contains(path))
        {
            return true;
        }

        QString ext = getFileExt(path);
        if(!ext.isEmpty() && fileIgnoreList_.contains(ext))
        {
            return true;
        }

        return false;
    }

}
