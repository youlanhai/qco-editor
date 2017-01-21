#ifndef EDITOR_BASE_EDITOR_H
#define EDITOR_BASE_EDITOR_H

#include <QObject>
#include "config.h"

namespace Editor
{
	class EDITOR_DLL BaseEditor : public QObject
	{
		Q_OBJECT
	public:
		BaseEditor();
		~BaseEditor();
	};
}

#endif //EDITOR_BASE_EDITOR_H
