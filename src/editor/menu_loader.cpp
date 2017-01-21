#include "menu_loader.h"
#include "runtime/json_helper.h"
#include "tools/log_tool.h"
#include "tools/json_tools.h"
#include "framework.h"

#include <QMenu>
#include <QAction>

DEFINE_LOG_COMPONENT(LOG_PRIORITY_DEBUG, "MenuLoader");

namespace Editor
{

    MenuLoader::MenuLoader(QObject *parent, QMenu *menu)
        : QObject(parent)
        , menu_(menu)
    {

    }

    bool MenuLoader::loadMenu(const std::string &fileName)
    {
        JsonValue document;
        if(!openJsonFile(fileName, document) || !document.isDict())
        {
            LOG_ERROR("Failed to load toolbox '%s'", fileName.c_str());
            return false;
        }

        const JsonValue &parameter = document["parameter"];
        if(parameter.isDict())
        {
            const mjson::Dict &dict = *parameter.rawDict();
            for(const mjson::NodePair &pair : dict)
            {
                parameters_[pair.key.asCString()] = pair.value;
            }
        }

        return loadMenuItem(document["menu"], menu_);
    }

    JsonValue MenuLoader::getParamter(const std::string &key) const
    {
        auto it = parameters_.find(key);
        if(it != parameters_.end())
        {
            return it->second;
        }
        return JsonValue();
    }

    void MenuLoader::emitCreateSignal(const std::string &key)
    {
        JsonValue parameter = getParamter(key);
        if(parameter.isNull())
        {
            LOG_ERROR("Failed to find configuration for menu '%s'", key.c_str());
            return;
        }

        emit signalCreateObject(parameter);
    }

    bool MenuLoader::loadMenuItem(const JsonHandle &config, QMenu *parentMenu)
    {
        if(!config.isArray())
        {
            return false;
        }

        if(config[0u].isString())
        {
            if(config[1u].isString())
            {
                QString text = QString::fromStdString(config[0u]->asStdString());
                QString name = QString::fromStdString(config[1u]->asStdString());

                QAction *action = new QAction(parentMenu);
                action->setObjectName(name);
                action->setText(text);
                connect(action, SIGNAL(triggered(bool)), this, SLOT(onMenuTrigger(bool)));
                parentMenu->addAction(action);
                return true;
            }
            else if(config[1u].isArray())
            {
                QString text = QString::fromStdString(config[0u]->asStdString());

                QMenu *menu = parentMenu->findChild<QMenu*>(text, Qt::FindChildrenRecursively);
                if(menu == nullptr)
                {
                    menu = new QMenu(parentMenu);
                    menu->setObjectName(text);
                    menu->setTitle(text);
                }

                if(!loadMenuItem(config[1u], menu))
                {
                    return false;
                }
                parentMenu->addAction(menu->menuAction());
                return true;
            }
        }
        else if(config[0u].isArray())
        {
            //如果第一项是数组，表示整个config的子项都是数组
            const mjson::Array &array = config->refArray();
            for(const JsonValue &value : array)
            {
                if(!loadMenuItem(value, parentMenu))
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    void MenuLoader::onMenuTrigger(bool /*checked*/)
    {
        QAction *action = qobject_cast<QAction*>(sender());
        if(action == nullptr)
        {
            return;
        }

        std::string key = action->objectName().toUtf8().data();
        LOG_DEBUG("menu create: %s", key.c_str());

        EDITOR_LOCK_GL_SCOPE();
        emitCreateSignal(key);
    }
}
