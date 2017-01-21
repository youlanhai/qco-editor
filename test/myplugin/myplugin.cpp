#include "myplugin.h"
#include "editor/tools/log_tool.h"

#include <2d/CCNode.h>

DEFINE_PLUGIN(myplugin, Editor::MyPlugin)

namespace Editor
{
	MyPlugin::MyPlugin()
	{

	}

	MyPlugin::~MyPlugin()
	{

	}

	bool MyPlugin::init()
	{
        PluginInterface::init();

        cocos2d::Node *p = cocos2d::Node::create();
        p->setName("Test My Plugin");

		LOG_INFO("My Plugin was loaded.");
		return true;
	}
}

