#ifndef RESOURCE_MIME_H
#define RESOURCE_MIME_H

#include "config.h"
#include <QStringList>

namespace Editor
{
    extern const char *MIME_STRING_LIST;

    EDITOR_DLL void parseMimeStringList(QStringList &out, const QByteArray &data);

    EDITOR_DLL void writeMimeStringList(QByteArray &out, const QStringList &data);
}

#endif // RESOURCE_MIME_H
