#ifndef NODE_TOOL_H
#define NODE_TOOL_H

#include "../config.h"
#include "../cc_predefine.h"

namespace Editor
{
    EDITOR_DLL int nodeGetIndex(cocos2d::Node *node);
    EDITOR_DLL void nodeSetIndex(cocos2d::Node *node, int index);
    EDITOR_DLL void nodeInsertChild(cocos2d::Node *node, cocos2d::Node *child, int index);
}

#endif // NODE_TOOL_H
