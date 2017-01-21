#include "node_tool.h"
#include <2d/CCNode.h>
#include <cocos2d.h>

namespace Editor
{
    void nodeUpdateChildrenArrival(cocos2d::Node *node)
    {
        auto &children = node->getChildren();
        for(cocos2d::Node *child : children)
        {
            node->reorderChild(child, child->getLocalZOrder());
        }
    }

    int nodeGetIndex(cocos2d::Node *node)
    {
        if(node->getParent() != nullptr)
        {
            return node->getParent()->getChildren().getIndex(node);
        }
        return -1;
    }

    void nodeSetIndex(cocos2d::Node *node, int index)
    {
        int oldIndex = nodeGetIndex(node);
        if(oldIndex < 0)
        {
            return;
        }

        auto &children = node->getParent()->getChildren();
        if(index < 0 || index >= children.size() || index == oldIndex)
        {
            return;
        }

        node->retain();
        children.erase(oldIndex);
        children.insert(index, node);
        node->release();

        nodeUpdateChildrenArrival(node);
    }

    void nodeInsertChild(cocos2d::Node *node, cocos2d::Node *child, int index)
    {
        auto &children = node->getChildren();
        if(index >= children.size())
        {
            node->addChild(child);
            return;
        }
        
        if(index < 0)
        {
            index = 0;
        }
        
        node->addChild(child);
        CCAssert(children.back() == child, "nodeInsertChild");
        
        child->retain();
        children.erase(children.end() - 1);
        children.insert(index, child);
        child->release();

        nodeUpdateChildrenArrival(node);
    }
}
