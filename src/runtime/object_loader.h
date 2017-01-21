#ifndef EDITOR_OBJECT_LOADER_H
#define EDITOR_OBJECT_LOADER_H

#include "pip_config.h"
#include <platform/CCPlatformMacros.h>

#include <unordered_map>
#include <map>
#include <string>

NS_CC_BEGIN
class Ref;
NS_CC_END
typedef cocos2d::Ref ObjectType;

class JsonHandle;

typedef void(*PropertySetter)(ObjectType*, const JsonHandle&, const JsonHandle&);
typedef JsonHandle(*PropertyGetter)(ObjectType*);

class PIP_DLL ObjectLoader
{
public:
    typedef ObjectLoader this_class;
    typedef std::unordered_map<std::string, PropertySetter> PropertySetterMap;
    typedef std::unordered_map<std::string, PropertyGetter> PropertyGetterMap;
    
public:
    ObjectLoader();
    virtual ~ObjectLoader();

    virtual const char* getLoaderName() const { return "ObjectLoader"; }

    virtual ObjectType* createObject(const JsonHandle &config) = 0;
    virtual bool setProperty(ObjectType *node, const std::string &name, const JsonHandle &value, const JsonHandle &config);

    virtual JsonHandle getProperty(ObjectType *node, const std::string &name);

    /** function was called when all it's property and children was loaded.*/
    virtual void postLoaded(ObjectType *node, const JsonHandle &config);
    
    /** return false if upgrade failed. */
    virtual bool upgradeProperty(JsonHandle &property);

    void registerPropertySetter(const std::string &propertyName, PropertySetter method);
    void registerPropertyGetter(const std::string &propertyName, PropertyGetter method);

protected:
    PropertySetterMap   setters_;
    PropertyGetterMap   getters_;
};


#define DECLARE_LOADER(CLASS) \
    typedef this_class   base_class; \
    typedef CLASS        this_class; \
    virtual const char*  getLoaderName() const override { return #CLASS; }

#define DEFINE_PROPERTY_SETTER(CLASS, PROPERTY_NAME) \
    static void node_set_##PROPERTY_NAME(CLASS *node, const JsonHandle &value, const JsonHandle &config)

#define DECLARE_PROPERTY_SETTER(CLASS, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
    DEFINE_PROPERTY_SETTER(CLASS, PROPERTY_NAME) \
    {\
        VALUE_TYPE val;\
        value >> val; \
        node->METHOD_NAME(val);\
    }

#define REGISTER_PROPERTY_SETTER(NAME) \
    registerPropertySetter(#NAME, (PropertySetter)node_set_##NAME)


#define DEFINE_PROPERTY_GETTER(CLASS, PROPERTY_NAME) \
    static JsonHandle node_get_##PROPERTY_NAME(CLASS *node)

#define DECLARE_PROPERTY_GETTER(CLASS, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
    DEFINE_PROPERTY_GETTER(CLASS, PROPERTY_NAME) \
    {\
        VALUE_TYPE val = node->METHOD_NAME();\
        return jsonFrom(val); \
    }

#define REGISTER_PROPERTY_GETTER(NAME) \
    registerPropertyGetter(#NAME, (PropertyGetter)node_get_##NAME)



#endif //EDITOR_OBJECT_LOADER_H
