#ifndef CMD_SET_NODE_PARENT_H
#define CMD_SET_NODE_PARENT_H

#include "../config.h"
#include <QUndoCommand>
#include "../cc_predefine.h"
#include <math/Vec3.h>

namespace Editor
{

    class EDITOR_DLL CmdReparentNode : public QUndoCommand
    {
    public:
        CmdReparentNode(NodePtr node, NodePtr newParent, int newIndex, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;

    private:
        NodePtr     node_;
        NodePtr     newParent_;
        NodePtr     oldParent_;
        int         newIndex_;
        int         oldIndex_;
        cocos2d::Vec3 worldPosition_;
    };

}

#endif // CMD_SET_NODE_PARENT_H
