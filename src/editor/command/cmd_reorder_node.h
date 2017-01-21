#ifndef CMD_REORDER_NODE_H
#define CMD_REORDER_NODE_H

#include "../config.h"
#include <QUndoCommand>
#include "../cc_predefine.h"

namespace Editor
{
    class EDITOR_DLL CmdReorderNode : public QUndoCommand
    {
    public:
        CmdReorderNode(NodePtr node, int newIndex, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;

    private:
        NodePtr     node_;
        int         newIndex_;
        int         oldIndex_;
    };
}

#endif // CMD_REORDER_NODE_H
