#ifndef QSTRINGTOOLS_H
#define QSTRINGTOOLS_H

#include "../config.h"
#include <QString>

namespace Editor
{

    EDITOR_DLL QString joinPath(const QString &a, const QString &b);
    EDITOR_DLL QString getFileName(const QString &path);
    EDITOR_DLL QString getFilePath(const QString &path);
    EDITOR_DLL QString getFileExt(const QString &path);
    EDITOR_DLL void removeFileExt(QString &path);
    EDITOR_DLL bool isSubPathOf(const QString &childPath, const QString &parentPath);

    EDITOR_DLL bool copyRecursively(const QString &srcFilePath, const QString &tgtFilePath);
}

#endif // QSTRINGTOOLS_H
