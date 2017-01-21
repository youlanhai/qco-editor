#ifndef DEFAULT_IMPORTER_H
#define DEFAULT_IMPORTER_H

#include "file_importer.h"

namespace Editor
{
    class EDITOR_DLL DefaultImporter : public FileImporter
    {
    public:
        DefaultImporter() = default;
        ~DefaultImporter() = default;

        virtual bool isAccepted(const QString &srcPath) const override;
        virtual bool execute(const QString &srcPath, const QString &dstPath) override;
    };
}
#endif // DEFAULT_IMPORTER_H
