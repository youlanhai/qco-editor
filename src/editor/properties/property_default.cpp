#include "property_default.h"
#include <runtime/json_helper.h>
#include "../tools/log_tool.h"

IMPLEMENT_SINGLETON(Editor::PropertyDefault);

namespace Editor
{

    PropertyDefault::PropertyDefault()
    {

    }

    PropertyDefault::~PropertyDefault()
    {
    }

    bool PropertyDefault::loadDefault(const std::string &fileName)
    {
        JsonValue doc;
        if(!openJsonFile(fileName, doc) || !doc.isDict())
        {
            return false;
        }

        const mjson::Dict &dict = *doc.rawDict();
        for(const auto &pair : dict)
        {
            if(pair.key.isString() && pair.value.isDict())
            {
                std::string type = pair.key.asStdString();
                valueMap_[type] = pair.value;
            }
        }
        return true;
    }

    JsonValue PropertyDefault::name2config(const std::string &name) const
    {
        auto it = valueMap_.find(name);
        if(it != valueMap_.end())
        {
            return it->second;
        }

        return JsonValue();
    }

} // end namespace Editor
