#include "json_helper.h"

#include <platform/CCFileUtils.h>
#include <2d/CCNode.h>

#include <smartjson/src/json.hpp>
#include <smartjson/src/parser.hpp>
#include <smartjson/src/writer.hpp>
#include <fstream>

#include "resource_holder.h"

#define CC_COLOR3B cocos2d::Color3B
#define CC_COLOR4B cocos2d::Color4B
#define CC_COLOR4F cocos2d::Color4F

const std::string       DefaultString;
const ResourceHolder    DefaultResouceHolder;
const cocos2d::Vec2     DefaultPoint;
const cocos2d::Size     DefaultSize;
const cocos2d::Rect     DefaultRect;
const cocos2d::Color4B   DefaultColor4B = CC_COLOR4B(0, 0, 0, 1);
const cocos2d::Color4F   DefaultColor4F = CC_COLOR4F(0, 0, 0, 1.0f);
const cocos2d::Color3B   DefaultColor3B = CC_COLOR3B(0, 0, 0);
const cocos2d::BlendFunc DefaultBlendFunc = {GL_ONE, GL_ZERO};
////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

static void formatRowCol(const char * str, int offset, int & row, int & col)
{
    row = col = 1;
    
    while(*str && offset-- > 0)
    {
        if(*str++ == '\n')
        {
            ++row;
            col = 0;
        }
        ++col;
    }
}

bool openJsonFile(const std::string & filename, JsonDocument & document)
{
    std::string data = cocos2d::FileUtils::getInstance()->getStringFromFile(filename);
    if(data.empty())
    {
        CCLOGERROR("Failed to open file %s", filename.c_str());
        return false;
    }

    mjson::Parser parser;
    int ret = parser.parse(data.c_str(), data.length());
    if(ret != mjson::RC_OK)
    {
        int r, c;
        formatRowCol(data.c_str(), parser.getErrorOffset(), r, c);

        CCLOGERROR("Failed to parse json file '%s'(%d: %d): %d",
                   filename.c_str(), r, c, ret);
        return false;
    }
    document = parser.getRoot();
    return true;
}

bool saveJsonFile(const std::string & filename, JsonValue &value)
{
    std::ofstream fout(filename.c_str());
    if(fout.bad())
    {
        CCLOGERROR("Failed to open file '%s' for write.", filename.c_str());
        return false;
    }

    mjson::Writer writer("    ");
    writer.write(value, fout);
    fout << '\n';
    fout.close();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////
bool json2bool(const JsonHandle &value, bool def)
{
    if(value.isBool())
    {
        return value->rawBool();
    }
    return def;
}

int json2int(const JsonHandle &value, int def)
{
    if(value.isNumber())
    {
        return value->asInt();
    }
    return def;
}

unsigned json2uint(const JsonHandle &value, unsigned def)
{
    if(value.isNumber())
    {
        return (unsigned)value->asInt();
    }
    return def;
}

int64_t json2int64(const JsonHandle &value, int64_t def)
{
    if(value.isNumber())
    {
        return value->asInt64();
    }
    return def;
}

uint64_t json2uint64(const JsonHandle &value, uint64_t def)
{
    if(value.isNumber())
    {
        return (uint64_t)value->asInt64();
    }
    return def;
}

float json2float(const JsonHandle &value, float def)
{
    return (float)json2double(value, def);
}

double json2double(const JsonHandle &value, double def)
{
    if(value.isNumber())
    {
        return value->asFloat();
    }
    return def;
}

std::string json2string(const JsonHandle &value, const std::string &def)
{
    if(value.isString())
    {
        return value->asStdString();
    }
    return def;
}

cocos2d::Rect json2rect(const JsonHandle &value, const cocos2d::Rect &def)
{
    if(value.isArray() && value->size() >= 4)
    {
        return cocos2d::Rect(
            json2float(value[0u]),
            json2float(value[1u]),
            json2float(value[2u]),
            json2float(value[3u])
            );
    }
    return def;
}

cocos2d::Size json2size(const JsonHandle &value, const cocos2d::Size &def)
{
    if(value.isArray() && value->size() >= 2)
    {
        return cocos2d::Size(
            json2float(value[0u], 0.0f),
            json2float(value[1u], 0.0f)
            );
    }
    return def;
}

cocos2d::Vec2 json2vec2(const JsonHandle &value, const cocos2d::Vec2 &def)
{
    if(value.isArray() && value->size() >= 2)
    {
        return cocos2d::Vec2(
            json2float(value[0u], 0.0f),
            json2float(value[1u], 0.0f)
            );
    }
    return def;
}

cocos2d::Vec3 json2vec3(const JsonHandle &value, const cocos2d::Vec3 &def)
{
    cocos2d::Vec3 ret = def;
    if(value.isArray())
    {
        ret.x = json2float(value[0u]);
        ret.y = json2float(value[1u]);
        ret.z = json2float(value[2u]);
    }
    return ret;
}

cocos2d::Vec4 json2vec4(const JsonHandle &value, const cocos2d::Vec4 &def)
{
    cocos2d::Vec4 ret = def;
    if(value.isArray())
    {
        ret.x = json2float(value[0u]);
        ret.y = json2float(value[1u]);
        ret.z = json2float(value[2u]);
        ret.w = json2float(value[3u]);
    }
    return ret;
}

cocos2d::Quaternion json2quaternion(const JsonHandle &value, const cocos2d::Quaternion &def)
{
    cocos2d::Quaternion ret = def;
    if(value.isArray())
    {
        ret.x = json2float(value[0u]);
        ret.y = json2float(value[1u]);
        ret.z = json2float(value[2u]);
        ret.w = json2float(value[3u]);
    }
    return ret;
}

cocos2d::Color3B json2color3b(const JsonHandle &value, const cocos2d::Color3B &def)
{
    if(value.isArray() && value->size() >= 3)
    {
        return CC_COLOR3B(
            json2int(value[0u], 0),
            json2int(value[1u], 0),
            json2int(value[2u], 0)
            );
    }
    return def;
}

cocos2d::Color4B json2color4b(const JsonHandle &value, const cocos2d::Color4B &def)
{
    if(value.isArray() && value->size() >= 4)
    {
        return CC_COLOR4B(
            json2int(value[0u], 0),
            json2int(value[1u], 0),
            json2int(value[2u], 0),
            json2int(value[3u], 0)
            );
    }
    return def;
}

cocos2d::Color4F json2color4f(const JsonHandle &value, const cocos2d::Color4F &def)
{
    if(value.isArray() && value->size() >= 4)
    {
        return CC_COLOR4F(
            json2float(value[0u], 0.0f),
            json2float(value[1u], 0.0f),
            json2float(value[2u], 0.0f),
            json2float(value[3u], 0.0f)
        );
    }
    return def;
}

cocos2d::BlendFunc json2blend(const JsonHandle &value, const cocos2d::BlendFunc &def)
{
    if(value.isArray() && value->size() >= 2)
    {
        cocos2d::BlendFunc ret = {
            (GLenum)json2uint(value[0u]),
            (GLenum)json2uint(value[1u])
        };
        return ret;
    }
    return def;
}

ResourceHolder json2resource(const JsonHandle &value, const ResourceHolder &def)
{
    return json2string(value, def);
}

std::string json2file(const JsonHandle &value, const std::string &def)
{
    std::string path = json2string(value, def);
    RESOLVE_FILE_RESOURCE(path);
    return path;
}

std::string json2text(const JsonHandle &value, const std::string &def)
{
    return UI_TEXT(json2string(value, def));
}

std::string json2font(const JsonHandle &value, const std::string &def)
{
    return UI_FONT(json2string(value, def));
}

////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////

void operator >> (const JsonHandle &value, bool &v)
{
    v = json2bool(value, false);
}

void operator >> (const JsonHandle &value, int8_t &v)
{
    v = (int8_t)json2int(value, 0);
}

void operator >> (const JsonHandle &value, uint8_t &v)
{
    v = (uint8_t)json2uint(value, 0);
}

void operator >> (const JsonHandle &value, int16_t &v)
{
    v = (int16_t)json2int(value, 0);
}

void operator >> (const JsonHandle &value, uint16_t &v)
{
    v = (uint16_t)json2uint(value, 0);
}

void operator >> (const JsonHandle &value, int &v)
{
    v = json2int(value, 0);
}

void operator >> (const JsonHandle &value, unsigned int &v)
{
    v = json2uint(value, 0);
}

void operator >> (const JsonHandle &value, int64_t &v)
{
    v = json2int64(value, 0);
}

void operator >> (const JsonHandle &value, uint64_t &v)
{
    v = json2uint64(value, 0);
}

void operator >> (const JsonHandle &value, float &v)
{
    v = json2float(value, 0.0f);
}

void operator >> (const JsonHandle &value, double &v)
{
    v = json2double(value, 0.0);
}

void operator >> (const JsonHandle &value, std::string &v)
{
    v = json2string(value, "");
}

void operator >> (const JsonHandle &value, cocos2d::Size &v)
{
    v = json2size(value);
}

void operator >> (const JsonHandle &value, cocos2d::Rect &v)
{
    v = json2rect(value);
}

void operator >> (const JsonHandle &value, cocos2d::Vec2 &v)
{
    v = json2size(value);
}

void operator >> (const JsonHandle &value, cocos2d::Vec3 &v)
{
    v = json2vec3(value, cocos2d::Vec3::ZERO);
}

void operator >> (const JsonHandle &value, cocos2d::Vec4 &v)
{
    v = json2vec4(value, cocos2d::Vec4::ZERO);
}

void operator >> (const JsonHandle &value, cocos2d::Quaternion &v)
{
    v = json2quaternion(value, cocos2d::Quaternion::ZERO);
}

void operator >> (const JsonHandle &value, cocos2d::Color3B &v)
{
    v = json2color3b(value);
}

void operator >> (const JsonHandle &value, cocos2d::Color4B &v)
{
    v = json2color4b(value);
}

void operator >> (const JsonHandle &value, cocos2d::Color4F &v)
{
    v = json2color4f(value);
}

void operator >> (const JsonHandle &value, cocos2d::BlendFunc &v)
{
    v = json2blend(value);
}

#ifdef BUILD_EDITOR
void operator >> (const JsonHandle &value, ResourceHolder &v)
{
    v = json2resource(value);
}
#endif

void operator >> (const JsonHandle &value, const char *&v)
{
    if(value.isString())
    {
        v = value->asCString();
    }
    else
    {
        v = "";
    }
}

////////////////////////////////////////////////////////////////////////////////
/// jsonFrom
////////////////////////////////////////////////////////////////////////////////
JsonValue jsonFrom(const cocos2d::Vec2 &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(2);
    p->append(val.x);
    p->append(val.y);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Vec3 &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(3);
    p->append(val.x);
    p->append(val.y);
    p->append(val.z);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Vec4 &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(4);
    p->append(val.x);
    p->append(val.y);
    p->append(val.z);
    p->append(val.w);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Quaternion &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(4);
    p->append(val.x);
    p->append(val.y);
    p->append(val.z);
    p->append(val.w);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Rect &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(4);
    p->append(val.origin.x);
    p->append(val.origin.y);
    p->append(val.size.width);
    p->append(val.size.height);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Size &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(2);
    p->append(val.width);
    p->append(val.height);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Color3B &val)
{
    return jsonFrom(cocos2d::Color4B(val));
}

JsonValue jsonFrom(const cocos2d::Color4B &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();;
    p->reserve(4);
    p->append(val.r);
    p->append(val.g);
    p->append(val.b);
    p->append(val.a);
    return ret;
}

JsonValue jsonFrom(const cocos2d::Color4F &val)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();;
    p->reserve(4);
    p->append(val.r);
    p->append(val.g);
    p->append(val.b);
    p->append(val.a);
    return ret;
}

JsonValue jsonFrom(const cocos2d::BlendFunc &v)
{
    JsonValue ret;
    mjson::Array *p = ret.setArray();
    p->reserve(2);
    p->append((int)v.src);
    p->append((int)v.dst);
    return ret;
}

////////////////////////////////////////////////////////////////////////////////
/// JsonHandle
////////////////////////////////////////////////////////////////////////////////
JsonHandle::JsonHandle(const value_type &p, const char *key)
{
    if(p.isDict())
    {
        mjson::Dict *dict = p.rawDict();
        mjson::Dict::iterator it = dict->find(key);
        if(it != dict->end())
        {
            val_ = it->value;
        }
    }
}

JsonHandle::JsonHandle(const value_type &p, size_t i)
{
    if(p.isArray() && i < p.size())
    {
        val_ = p[i];
    }
}

void JsonHandle::setMember(const char *key, const std::string &value)
{
    JsonValue val(value);
    setMember(key, val);
}

void JsonHandle::setMember(const char *key, const char *value)
{
    JsonValue val(value);
    setMember(key, val);
}

void JsonHandle::setMember(const char *key, JsonHandle v)
{
    if(isObject())
    {
        val_[key] = *v;
    }
}
