#ifndef RESOURCE_MAPPING_H
#define RESOURCE_MAPPING_H

#include "pip_config.h"
#include "singleton.h"
#include <unordered_map>
#include <string>

class PIP_DLL StringMapping
{
public:
    typedef std::unordered_map<std::string, std::string> StringMap;

    StringMapping();
    ~StringMapping();

    bool addConfigFile(const std::string &filename);
    void clear();

    const std::string& value(const std::string &key) const;
    void addValue(const std::string &key, const std::string &val);

    const StringMap& values() const { return values_; }

private:
    StringMap       values_;
};

/** 资源路径映射，便于多语言化。
 *  通过一个key来查找对应的值，如果存在，则返回该值，否则返回key本身。
 *  比如文字：编辑器使用中文编辑，需要转换成其他国家语言时，增加一个配置文件，
 * 将中文转换成其他国家语言。程序运行的过程中，优先从配置文件中查找文字，如果找到就返回翻译后的文字，
 * 否则，返回中文本身。
 */
class PIP_DLL ResourceMapping : public Singleton<ResourceMapping>
{
public:
    ResourceMapping();
    ~ResourceMapping();

    StringMapping   text;
    StringMapping   font;
    StringMapping   file;
};


inline const std::string& UI_TEXT(const std::string &key)
{
    return ResourceMapping::instance()->text.value(key);
}

inline const std::string& UI_FONT(const std::string &key)
{
    return ResourceMapping::instance()->font.value(key);
}

inline const std::string& UI_FILE(const std::string &key)
{
    return ResourceMapping::instance()->file.value(key);
}

#endif // RESOURCE_MAPPING_H
