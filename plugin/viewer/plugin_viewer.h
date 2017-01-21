#ifndef PLUGIN_VIEWER_H
#define PLUGIN_VIEWER_H

#include "editor/plugin_interface.h"

class QAction;
class ViewerWindow;

namespace Editor
{
	class PluginViewer : public PluginInterface
	{
        Q_OBJECT
	public:
		PluginViewer();
		~PluginViewer();

        virtual bool init() override;

    private slots:
        void onActionToggleView();
        void onViewDestroy();

    private:

        QAction*    actionToggleView_;
        ViewerWindow* view_;
	};
}

#endif //PLUGIN_VIEWER_H
