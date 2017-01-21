#ifndef __Editor__EditorTools__
#define __Editor__EditorTools__

#include "../config.h"
#include <QVariant>
#include <QRect>
#include <QPoint>
#include <QSize>

#include "runtime/json_helper.h"

namespace Editor
{
    EDITOR_DLL void json2tvalue(QVariant &output, const JsonValue &input);
    EDITOR_DLL void tvalue2json(JsonValue &output, const QVariant &input);

    EDITOR_DLL void jsonAddMember(JsonValue &value, const char *key, const JsonValue &val);
    EDITOR_DLL void jsonAddMember(JsonValue &value, const char *key, const std::string &val);
    EDITOR_DLL void jsonAddMember(JsonValue &value, const char *key, const QString &val);

    template<typename T>
    void jsonAddMember(JsonValue &value, const char *key, T val)
    {
        jsonAddMember(value, key, mjson::Node(val));
    }
}

EDITOR_DLL JsonValue jsonFrom(const QString &val);
EDITOR_DLL JsonValue jsonFrom(const JsonValue &val);
EDITOR_DLL JsonValue jsonFrom(const QRect &val);
EDITOR_DLL JsonValue jsonFrom(const QPoint &val);
EDITOR_DLL JsonValue jsonFrom(const QSize &val);

EDITOR_DLL QString json2qstring(const JsonHandle &value, QString def = "");
EDITOR_DLL QRect json2qrect(const JsonHandle &value, QRect def = QRect());
EDITOR_DLL QSize json2qsize(const JsonHandle &value, QSize def = QSize());
EDITOR_DLL QPoint json2qpoint(const JsonHandle &value, QPoint def = QPoint());

EDITOR_DLL void operator >> (const JsonHandle &value, QString &v);

#endif /* defined(__Editor__EditorTools__) */
