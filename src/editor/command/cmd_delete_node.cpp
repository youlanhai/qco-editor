#include "cmd_delete_node.h"
#include "../framework.h"
#include "../document.h"
#include "../tools/node_tool.h"
#include <2d/CCNode.h>

namespace Editor
{
    CmdDeleteNode::CmdDeleteNode(NodePtr node, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(node)
        , parent_(node_->getParent())
        , index_(nodeGetIndex(node))
    {
        gatherNodeConfig(node);
    }

    void CmdDeleteNode::undo()
    {
        applyNodeConfig(node_);
        Framework::instance()->addNodePrivate(node_, parent_, index_);
    }

    void CmdDeleteNode::redo()
    {
        Framework::instance()->removeNode(node_);
    }

    void CmdDeleteNode::gatherNodeConfig(cocos2d::Node *node)
    {
        mjson::Node config = Framework::instance()->document_->findNodeConfig(node);
        if(config.isNull())
        {
            return;
        }
        configs_[node] = config;

        auto &children = node->getChildren();
        for(cocos2d::Node *child : children)
        {
            gatherNodeConfig(child);
        }
    }

    void CmdDeleteNode::applyNodeConfig(cocos2d::Node *node)
    {
        auto it = configs_.find(node);
        if(it == configs_.end())
        {
            return;
        }

        Framework::instance()->document_->addNode(node, it->second);

        auto &children = node->getChildren();
        for(cocos2d::Node *child : children)
        {
            applyNodeConfig(child);
        }
    }
}
