#ifndef PROPERTY_PARAM_H
#define PROPERTY_PARAM_H

#include "../config.h"
#include <runtime/json_helper.h>
#include <base/CCRef.h>

class EDITOR_DLL PropertyParam
{
public:
    typedef cocos2d::Ref*   ObjectPointer;
    PropertyParam(ObjectPointer _object,
                  const std::string & _name,
                  const JsonHandle &_value,
                  const JsonHandle &_properties);

    ObjectPointer           object;
    std::string             name;
    JsonHandle              value;
    JsonHandle              properties;
};


#endif // PROPERTY_PARAM_H
