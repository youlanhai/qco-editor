#ifndef RUNTIME_JSON_HELPER_H
#define RUNTIME_JSON_HELPER_H

#include "pip_config.h"

#include <ccTypes.h>
#include <math/CCGeometry.h>

#include <smartjson/src/json.hpp>
#include "resource_holder.h"

typedef mjson::Node 		JsonDocument;
typedef mjson::IAllocator 	JsonAllocator;
typedef mjson::Node 		JsonValue;

PIP_DLL bool openJsonFile(const std::string & filename, JsonDocument &document);
PIP_DLL bool saveJsonFile(const std::string & filename, JsonValue &value);

class JsonHandle;

PIP_DLL extern const std::string    DefaultString;
PIP_DLL extern const ResourceHolder DefaultResouceHolder;
PIP_DLL extern const cocos2d::Vec2  DefaultPoint;
PIP_DLL extern const cocos2d::Size  DefaultSize;
PIP_DLL extern const cocos2d::Rect  DefaultRect;
PIP_DLL extern const cocos2d::Color4B   DefaultColor4B;
PIP_DLL extern const cocos2d::Color4F   DefaultColor4F;
PIP_DLL extern const cocos2d::Color3B   DefaultColor3B;
PIP_DLL extern const cocos2d::BlendFunc DefaultBlendFunc;
//////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////
PIP_DLL bool                json2bool       (const JsonHandle &value, bool def = false);
PIP_DLL int                 json2int        (const JsonHandle &value, int def = 0);
PIP_DLL unsigned            json2uint       (const JsonHandle &value, unsigned def = 0);
PIP_DLL int64_t             json2int64      (const JsonHandle &value, int64_t def = 0);
PIP_DLL uint64_t            json2uint64     (const JsonHandle &value, uint64_t def = 0);
PIP_DLL float               json2float      (const JsonHandle &value, float def = 0.0f);
PIP_DLL double              json2double     (const JsonHandle &value, double def = 0.0);
PIP_DLL std::string         json2string     (const JsonHandle &value, const std::string &def = DefaultString);
PIP_DLL cocos2d::Rect       json2rect       (const JsonHandle &value, const cocos2d::Rect &def = DefaultRect);
PIP_DLL cocos2d::Size       json2size       (const JsonHandle &value, const cocos2d::Size &def = DefaultSize);
PIP_DLL cocos2d::Vec2       json2vec2       (const JsonHandle &value, const cocos2d::Vec2 &def = DefaultPoint);
PIP_DLL cocos2d::Vec3       json2vec3       (const JsonHandle &value, const cocos2d::Vec3 &def = cocos2d::Vec3::ZERO);
PIP_DLL cocos2d::Vec4       json2vec4       (const JsonHandle &value, const cocos2d::Vec4 &def = cocos2d::Vec4::ZERO);
PIP_DLL cocos2d::Quaternion json2quaternion (const JsonHandle &value, const cocos2d::Quaternion &def = cocos2d::Quaternion::ZERO);

PIP_DLL cocos2d::Color3B    json2color3b    (const JsonHandle &value, const cocos2d::Color3B &def = DefaultColor3B);
PIP_DLL cocos2d::Color4B    json2color4b    (const JsonHandle &value, const cocos2d::Color4B &def = DefaultColor4B);
PIP_DLL cocos2d::Color4F    json2color4f    (const JsonHandle &value, const cocos2d::Color4F &def = DefaultColor4F);
PIP_DLL cocos2d::BlendFunc  json2blend      (const JsonHandle &value, const cocos2d::BlendFunc &def = DefaultBlendFunc);

PIP_DLL ResourceHolder      json2resource   (const JsonHandle &value, const ResourceHolder &def = DefaultResouceHolder);
PIP_DLL std::string         json2file       (const JsonHandle &value, const std::string &def = DefaultString);
PIP_DLL std::string         json2text       (const JsonHandle &value, const std::string &def = DefaultString);
PIP_DLL std::string         json2font       (const JsonHandle &value, const std::string &def = DefaultString);

//////////////////////////////////////////////////////////////////////
///
//////////////////////////////////////////////////////////////////////
PIP_DLL void operator >> (const JsonHandle &value, bool &v);
PIP_DLL void operator >> (const JsonHandle &value, int8_t &v);
PIP_DLL void operator >> (const JsonHandle &value, uint8_t &v);
PIP_DLL void operator >> (const JsonHandle &value, int16_t &v);
PIP_DLL void operator >> (const JsonHandle &value, uint16_t &v);
PIP_DLL void operator >> (const JsonHandle &value, int &v);
PIP_DLL void operator >> (const JsonHandle &value, unsigned int &v);
PIP_DLL void operator >> (const JsonHandle &value, int64_t &v);
PIP_DLL void operator >> (const JsonHandle &value, uint64_t &v);
PIP_DLL void operator >> (const JsonHandle &value, float &v);
PIP_DLL void operator >> (const JsonHandle &value, double &v);
PIP_DLL void operator >> (const JsonHandle &value, std::string &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Size &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Rect &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Vec2 &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Vec3 &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Vec4 &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Quaternion &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Color3B &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Color4B &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::Color4F &v);
PIP_DLL void operator >> (const JsonHandle &value, cocos2d::BlendFunc &v);
PIP_DLL void operator >> (const JsonHandle &value, const char *&v);

#ifdef BUILD_EDITOR
PIP_DLL void operator >> (const JsonHandle &value, ResourceHolder &v);
#endif

//////////////////////////////////////////////////////////////////////
/// value to json
//////////////////////////////////////////////////////////////////////
inline JsonValue jsonFrom(bool v){ return JsonValue(v); }
inline JsonValue jsonFrom(int8_t v){ return JsonValue((int)v); }
inline JsonValue jsonFrom(uint8_t v){ return JsonValue((unsigned)v); }
inline JsonValue jsonFrom(int16_t v){ return JsonValue((int)v); }
inline JsonValue jsonFrom(uint16_t v){ return JsonValue((unsigned)v); }
inline JsonValue jsonFrom(int v){ return JsonValue(v); }
inline JsonValue jsonFrom(unsigned int v){ return JsonValue(v); }
inline JsonValue jsonFrom(int64_t v){ return JsonValue(v); }
inline JsonValue jsonFrom(uint64_t v){ return JsonValue(v); }
inline JsonValue jsonFrom(float v){ return JsonValue(v); }
inline JsonValue jsonFrom(double v){ return JsonValue(v); }

inline JsonValue jsonFrom(const char *val){ return JsonValue(val); }
inline JsonValue jsonFrom(const std::string &val){ return JsonValue(val.c_str(), val.size()); }

PIP_DLL JsonValue jsonFrom(const cocos2d::Vec2 &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Rect &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Size &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Color3B &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Color4B &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Color4F &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::BlendFunc &v);
PIP_DLL JsonValue jsonFrom(const cocos2d::Vec3 &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Vec4 &val);
PIP_DLL JsonValue jsonFrom(const cocos2d::Quaternion &val);

//////////////////////////////////////////////////////////////////////
/// \brief The JsonHandle class
///
//////////////////////////////////////////////////////////////////////
class PIP_DLL JsonHandle
{
public:
    typedef JsonValue value_type;

    JsonHandle()
    {}

    JsonHandle(const value_type &p)
        : val_(p)
    {}

    JsonHandle(const value_type *p)
    {
        if(p != 0)
        {
            val_ = *p;
        }
    }

    JsonHandle(const JsonHandle &v)
        : val_(v.val_)
    {}

    JsonHandle(const value_type &p, const char *key);
    JsonHandle(const value_type &p, size_t i);

    const JsonHandle& operator = (const JsonHandle &v)
    {
        val_ = v.val_;
        return *this;
    }

    const JsonHandle& operator = (const value_type *p)
    {
        if(p != 0)
        {
            val_ = *p;
        }
        else
        {
            val_.setNull();
        }
        return *this;
    }

    const JsonHandle& operator = (const value_type &p)
    {
        val_ = p;
        return *this;
    }

    JsonHandle operator[] (unsigned int i) const
    {
        return JsonHandle(val_, i);
    }

    JsonHandle operator[] (const char *key) const
    {
        return JsonHandle(val_, key);
    }

    const value_type& operator* () const
    {
        return val_;
    }

    value_type& operator* ()
    {
        return val_;
    }

    const value_type* operator-> () const
    {
        return &val_;
    }

    value_type* operator-> ()
    {
        return &val_;
    }

    bool isNull()   const { return val_.isNull(); }
    bool isBool()   const { return val_.isBool(); }
    bool isInt()    const { return val_.isInt(); }
    bool isUint()   const { return val_.isInt(); }
    bool isInt64()  const { return val_.isInt(); }
    bool isUint64() const { return val_.isInt(); }
    bool isDouble() const { return val_.isFloat(); }
    bool isNumber() const { return val_.isNumber(); }
    bool isString() const { return val_.isString(); }
    bool isObject() const { return val_.isDict(); }
    bool isArray()  const { return val_.isArray();  }
    
    void setMember(const char *key, const std::string &value);
    void setMember(const char *key, const char *value);
    void setMember(const char *key, JsonHandle v);

    bool isDict() const { return isObject(); }

public:
    value_type      val_;
};

#endif /* defined(RUNTIME_JSON_HELPER_H) */

