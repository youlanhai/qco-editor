#ifndef FILE_IMPORTER_H
#define FILE_IMPORTER_H

#include "../config.h"
#include <QString>

namespace Editor
{
    class EDITOR_DLL FileImporter
    {
    public:
        FileImporter() = default;
        virtual ~FileImporter() = default;

        virtual bool isAccepted(const QString &srcPath) const = 0;

        virtual bool execute(const QString &srcPath, const QString &dstPath) = 0;
    };
}

#endif //FILE_IMPORTER_H
