#include "json_tools.h"
#include "runtime/json_helper.h"

#include <QList>
#include <QMap>
#include <QColor>


namespace Editor
{
    void json2tvalue(QVariant & output, const JsonValue &input)
    {
        if(input.isNull())
        {
            output.clear();
        }
        else if(input.isInt())
        {
            int64_t v = input.asInt64();
            if(v >= std::numeric_limits<int32_t>::min() && v <= std::numeric_limits<int32_t>::max())
            {
                output = QVariant((int)v);
            }
            else
            {
                output = QVariant((qlonglong)v);
            }
        }
        else if(input.isFloat())
        {
            output = QVariant(input.asFloat());
        }
        else if(input.isBool())
        {
            output = QVariant(input.asBool());
        }
        else if(input.isString())
        {
            output = QVariant(QString::fromUtf8(input.asCString(), input.size()));
        }
        else if(input.isArray())
        {
            QVariantList list;
            list.reserve(input.size());

            const mjson::Array &array = *(input.rawArray());
            for(const JsonValue &v : array)
            {
                QVariant tmp;
                json2tvalue(tmp, v);
                list.append(tmp);
            }
            output = list;
        }
        else if(input.isDict())
        {
            QMap<QString, QVariant> map;

            const mjson::Dict &dict = *(input.rawDict());
            for(const auto &pair : dict)
            {
                QString qstring = QString::fromUtf8(pair.key.asCString(), pair.key.size());
                QVariant qvalue;
                json2tvalue(qvalue, pair.value);

                map.insert(qstring, qvalue);
            }

            output = map;
        }
        else
        {
            output.clear();
            assert(false && "shouldn't execute to here.");
        }
    }
    
    void tvalue2json(JsonValue &output, const QVariant &input)
    {
        switch(input.type())
        {
        case QVariant::Invalid:
        {
            output.setNull();
            break;
        }
        case QVariant::Bool:
        {
            output = input.toBool();
            break;
        }
        case QVariant::Int:
        {
            output = input.toInt();
            break;
        }
        case QVariant::LongLong:
        {
            output = input.toLongLong();
            break;
        }
        case QVariant::Double:
        {
            output = input.toDouble();
            break;
        }
        case QVariant::String:
        {
            QByteArray str = input.toString().toUtf8();
            output.setString(str.data(), str.size());
            break;
        }
        case QVariant::StringList:
        {
            QStringList list = input.toStringList();

            mjson::Array *array = output.setArray();
            array->reserve(list.size());

            foreach(const QString &qstring, list)
            {
                QByteArray str = qstring.toUtf8();
                array->append(mjson::Node(str.data(), str.size()));
            }
            break;
        }
        case QVariant::List:
        {
            QVariantList list = input.toList();

            mjson::Array *array = output.setArray();
            array->reserve(list.size());

            JsonValue temp;
            foreach(const QVariant &qvalue, list)
            {
                tvalue2json(temp, qvalue);
                array->append(temp);
            }
            break;
        }
        case QVariant::Map:
        {
            output.setDict();
            JsonValue tempV;

            QMap<QString, QVariant> qmap = input.toMap();
            for(QMap<QString, QVariant>::const_iterator it = qmap.begin(); it != qmap.end(); ++it)
            {
                std::string key = it.key().toStdString();
                tvalue2json(tempV, it.value());
                output[key] = tempV;
            }
            break;
        }
        case QVariant::Color:
        {
            QColor pt = input.value<QColor>();

            mjson::Array* array = output.setArray();
            array->reserve(4);

            array->append(pt.red());
            array->append(pt.green());
            array->append(pt.blue());
            array->append(pt.alpha());
            break;
        }
        default:
        {
            output.setNull();
            assert(false && "shuldn't execute to here.");
        }
        }
    }

    void jsonAddMember(JsonValue &value, const char *key, const JsonValue &val)
    {
        value[key] = val;
    }

    void jsonAddMember(JsonValue &value, const char *key, const std::string &val)
    {
        jsonAddMember(value, key, mjson::Node(val));
    }

    void jsonAddMember(JsonValue &value, const char *key, const QString &val)
    {
        jsonAddMember(value, key, mjson::Node(val.toStdString()));
    }

} // end namespace Editor


JsonValue jsonFrom(const JsonValue &val)
{
    return val.clone();
}

JsonValue jsonFrom(const QString &val)
{
    QByteArray str = val.toUtf8();
    return JsonValue(str.data(), str.size());
}

JsonValue jsonFrom(const QRect &val)
{
    JsonValue ret;
    mjson::Array *arr = ret.setArray();
    arr->append(val.x());
    arr->append(val.y());
    arr->append(val.width());
    arr->append(val.height());
    return ret;
}

JsonValue jsonFrom(const QPoint &val)
{
    JsonValue ret;
    mjson::Array *arr = ret.setArray();
    arr->append(val.x());
    arr->append(val.y());
    return ret;
}

JsonValue jsonFrom(const QSize &val)
{
    JsonValue ret;
    mjson::Array *arr = ret.setArray();
    arr->append(val.width());
    arr->append(val.height());
    return ret;
}


QString json2qstring(const JsonHandle &value, QString def)
{
    if(value.isString())
    {
        return QString::fromStdString(value->asStdString());
    }
    return def;
}

QRect json2qrect(const JsonHandle &value, QRect def)
{
    if(value.isArray() && value->size() == 4)
    {
        return QRect(value[0u]->asInt(),
                value[1]->asInt(),
                value[2]->asInt(),
                value[3]->asInt()
                );
    }
    return def;
}

QSize json2qsize(const JsonHandle &value, QSize def)
{
    if(value.isArray() && value->size() == 2)
    {
        return QSize(value[0u]->asInt(),
                value[1]->asInt()
                );
    }
    return def;
}

QPoint json2qpoint(const JsonHandle &value, QPoint def)
{
    if(value.isArray() && value->size() == 2)
    {
        return QPoint(value[0u]->asInt(),
                value[1]->asInt()
                );
    }
    return def;
}

void operator >> (const JsonHandle &value, QString &v)
{
    v = json2qstring(value);
}
