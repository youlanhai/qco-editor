#ifndef CMD_DELETE_COMPONENT_H
#define CMD_DELETE_COMPONENT_H


#include "../config.h"
#include <QUndoCommand>
#include <smartjson/src/json.hpp>
#include <map>
#include "../cc_predefine.h"

namespace Editor
{
    class EDITOR_DLL CmdDeleteComponent : public QUndoCommand
    {
    public:
        CmdDeleteComponent(ComponentPtr component, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;
    private:

        NodePtr     node_;
        ComponentPtr component_;
        mjson::Node config_;
    };
}

#endif // CMD_DELETE_COMPONENT_H
