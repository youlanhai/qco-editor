#include "cmd_reparent_node.h"
#include <2d/CCNode.h>
#include "../tools/node_tool.h"
#include "../hierarchy.h"
#include "../framework.h"

namespace Editor
{
    CmdReparentNode::CmdReparentNode(NodePtr node, NodePtr newParent, int newIndex, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(node)
        , newParent_(newParent)
        , oldParent_(node->getParent())
        , newIndex_(newIndex)
        , oldIndex_(nodeGetIndex(node))
    {
        worldPosition_ = node->getPosition3D();
        oldParent_->getNodeToWorldTransform().transformPoint(&worldPosition_);
    }

    void CmdReparentNode::undo()
    {
        newParent_->removeChild(node_);
        Framework::instance()->hierarchy_->sortNodeChildren(newParent_);

        nodeInsertChild(oldParent_, node_, oldIndex_);
        Framework::instance()->hierarchy_->sortNodeChildren(oldParent_);

        cocos2d::Vec3 position = worldPosition_;
        oldParent_->getWorldToNodeTransform().transformPoint(&position);
        node_->setPosition3D(position);
        Framework::instance()->emitNodePropertyChange(node_, "position", jsonFrom(position));
    }

    void CmdReparentNode::redo()
    {
        oldParent_->removeChild(node_);
        Framework::instance()->hierarchy_->sortNodeChildren(oldParent_);

        nodeInsertChild(newParent_, node_, newIndex_);
        Framework::instance()->hierarchy_->sortNodeChildren(newParent_);

        cocos2d::Vec3 position = worldPosition_;
        newParent_->getWorldToNodeTransform().transformPoint(&position);
        node_->setPosition3D(position);
        Framework::instance()->emitNodePropertyChange(node_, "position", jsonFrom(position));
    }
}
