#ifndef TOOLBOX_H
#define TOOLBOX_H

#include "config.h"
#include <QObject>
#include <unordered_map>
#include <string>

#include <smartjson/src/json.hpp>
class JsonHandle;
typedef mjson::Node JsonValue;

class QMenu;

namespace Editor
{

    class EDITOR_DLL MenuLoader : public QObject
    {
        Q_OBJECT
    public:
        MenuLoader(QObject *parent, QMenu *menu);

        bool loadMenu(const std::string &fileName);
        JsonValue getParamter(const std::string &key) const;

        void emitCreateSignal(const std::string &key);

    signals:
        void signalCreateObject(const JsonValue &config);

    public slots:
        void onMenuTrigger(bool checked = false);

    private:
        bool loadMenuItem(const JsonHandle &config, QMenu *parentMenu);

        QMenu*      menu_;

        std::unordered_map<std::string, JsonValue> parameters_;
    };

} // end namespace Editor

#endif // TOOLBOX_H
