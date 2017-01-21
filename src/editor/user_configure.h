#ifndef USER_CONFIGURE_H
#define USER_CONFIGURE_H

#include "config.h"
#include <QString>
#include <runtime/singleton.h>
#include <smartjson/src/node.hpp>

namespace Editor
{
    class EDITOR_DLL UserConfigure : public Singleton<UserConfigure>
    {
    public:
        UserConfigure();

        bool load(const QString &path);
        bool save();

        bool applyConfig(const std::string &key, const mjson::Node &value);
        void setConfig(const std::string &key, const mjson::Node &value);

        const mjson::Node& getConfig() const { return config_; }

    private:
        std::string     filePath_;
        mjson::Node     config_;
    };

}

#endif // USER_CONFIGURE_H
