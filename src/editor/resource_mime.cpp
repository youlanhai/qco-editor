#include "resource_mime.h"

#include <runtime/binary_writer.h>
#include <runtime/binary_reader.h>

namespace Editor
{
    const char *MIME_STRING_LIST = "editor/string-list";

    void parseMimeStringList(QStringList &out, const QByteArray &data)
    {
        pip::ReaderSameEndian reader;
        reader.assign((const unsigned char*)data.data(), data.size());

        int length;
        while(!reader.empty())
        {
            length = reader.readInt16();
            const char *str = (const char*)reader.skip(length);
            if(reader.valid())
            {
                out.append(QString::fromUtf8(str, length));
            }
        }
    }

    void writeMimeStringList(QByteArray &out, const QStringList &data)
    {
        pip::WriterSameEndian writer;
        foreach(const QString &str, data)
        {
            QByteArray u = str.toUtf8();
            writer.writeInt16(u.size());
            writer.writeBuffer(u.data(), u.size());
        }
        out.append((const char *)writer.begin(), writer.size());
    }
}
