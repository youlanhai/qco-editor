#ifndef IMPORTER_H
#define IMPORTER_H

#include "config.h"
#include <QObject>
#include <QString>

#include <vector>

class JsonHandle;
namespace Editor
{
    class FileImporter;

    struct EDITOR_DLL ExportTask
    {
        QString srcPath;
        QString dstPath;

        ExportTask()
        {}

        ExportTask(QString src, QString dst)
            : srcPath(src)
            , dstPath(dst)
        {}
    };

    struct EDITOR_DLL ImportTask
    {
        enum Type
        {
            COPY,
            RENAME,
        };

        QString srcPath;
        QString dstPath;
        Type type;

        ImportTask(){}
        ImportTask(Type tp, const QString &src, const QString &dst)
            : type(tp)
            , srcPath(src)
            , dstPath(dst)
        {}
    };


    class EDITOR_DLL Importer : public QObject
    {
        Q_OBJECT
    public:
        explicit Importer(QObject *parent = 0);
        ~Importer();

        //导出文件或目录，path相对于assets
        bool exportAssets(const QString &path = "");
        bool exportAssets(const QStringList &srcFileList);

        /** 导入资源。
         * @param srcPath 可以是目录或文件，必须是绝对路径。
         * @param dstPath 是相对于assets的路径。
         */
        bool importAssets(const QString &srcPath, const QString &dstPath);

        bool importAssets(const QStringList &srcFileList, const QString &dstPath);

        //path是相对路径
        bool convertUUIDToPath(const QString &path);

        //path是相对路径
        bool convertPathToUUID(const QString &path);

    private:
        //srcPath和dstPath必须是绝对路径
        bool exportFile(const QString &srcPath, const QString &dstPath);
        //srcPath和dstPath必须是绝对路径
        bool importFile(const QString &srcPath, const QString &dstPath);

        bool processExportTasks(QList<ExportTask> tasks);
        bool processImportTasks(QList<ImportTask> tasks);

        std::vector<FileImporter*>  importers_;
        std::vector<FileImporter*>  exporters_;
    };

} // end namespace Editor

#endif // IMPORTER_H
