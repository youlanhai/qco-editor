#ifndef EDITORPROPERTYDEFAULT_H
#define EDITORPROPERTYDEFAULT_H

#include "../config.h"
#include "runtime/singleton.h"
#include <smartjson/src/json.hpp>

#include <vector>
#include <map>

typedef mjson::Node JsonValue;
namespace Editor
{

    class EDITOR_DLL PropertyDefault : public Singleton<PropertyDefault>
    {
    public:
        PropertyDefault();
        ~PropertyDefault();

        bool loadDefault(const std::string &fileName);
        JsonValue name2config(const std::string &name) const;

    private:
        typedef std::map<std::string, JsonValue> ValueMap;
        ValueMap        valueMap_;
    };

} // end namespace Editor

#endif // EDITORPROPERTYDEFAULT_H
