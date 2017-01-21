#include "default_importer.h"

#include <QFile>

namespace Editor
{
    bool DefaultImporter::isAccepted(const QString &srcPath) const
    {
        return true;
    }

    bool DefaultImporter::execute(const QString &srcPath, const QString &dstPath)
    {
        if(srcPath != dstPath)
        {
            if(QFile::exists(dstPath))
            {
                QFile::remove(dstPath);
            }
            return QFile::copy(srcPath, dstPath);
        }
        return true;
    }
}
