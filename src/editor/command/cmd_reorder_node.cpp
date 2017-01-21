#include "cmd_reorder_node.h"
#include "../tools/node_tool.h"
#include "../hierarchy.h"
#include "../framework.h"

#include <2d/CCNode.h>

namespace Editor
{
    CmdReorderNode::CmdReorderNode(NodePtr node, int newIndex, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(node)
        , newIndex_(newIndex)
    {
        oldIndex_ = nodeGetIndex(node);
    }

    void CmdReorderNode::undo()
    {
        nodeSetIndex(node_, oldIndex_);
        Framework::instance()->hierarchy_->sortNodeChildren(node_->getParent());
    }

    void CmdReorderNode::redo()
    {
        nodeSetIndex(node_, newIndex_);
        Framework::instance()->hierarchy_->sortNodeChildren(node_->getParent());
    }

}
