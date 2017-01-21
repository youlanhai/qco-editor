#ifndef H_MY_PLUGIN_H
#define H_MY_PLUGIN_H

#include "editor/plugin_interface.h"

namespace Editor
{
	class MyPlugin : public PluginInterface
	{
	public:
		MyPlugin();
		~MyPlugin();

		virtual bool init() override;
	};
}

#endif //H_MY_PLUGIN_H
