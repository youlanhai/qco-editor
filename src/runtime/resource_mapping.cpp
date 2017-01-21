#include "resource_mapping.h"
#include "json_helper.h"

StringMapping::StringMapping()
{

}

StringMapping::~StringMapping()
{

}

bool StringMapping::addConfigFile(const std::string &filename)
{
    JsonDocument doc;
    if(!openJsonFile(filename, doc) || !doc.isDict())
    {
        return false;
    }

    const mjson::Dict &dict = doc.refDict();
    for(const mjson::NodePair &pair : dict)
    {
        values_[pair.key.asStdString()] = pair.value.asStdString();
    }
    return true;
}

void StringMapping::clear()
{
    values_.clear();
}

const std::string& StringMapping::value(const std::string &key) const
{
    auto it = values_.find(key);
    if(it != values_.end())
    {
        return it->second;
    }

    return key;
}

void StringMapping::addValue(const std::string &key, const std::string &val)
{
    values_[key] = val;
}

//////////////////////////////////////////////////////////////////////
IMPLEMENT_SINGLETON(ResourceMapping);

ResourceMapping::ResourceMapping()
{

}

ResourceMapping::~ResourceMapping()
{

}
