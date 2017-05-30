#ifndef CMD_DELETE_NODE_H
#define CMD_DELETE_NODE_H

#include "../config.h"
#include <QUndoCommand>
#include <smartjson/src/json.hpp>
#include <map>
#include "../cc_predefine.h"

namespace Editor
{
    class EDITOR_DLL CmdDeleteNode : public QUndoCommand
    {
    public:
        CmdDeleteNode(NodePtr node, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;
    private:
        void gatherNodeConfig(cocos2d::Node *node);
        void applyNodeConfig(cocos2d::Node *node);

        NodePtr     node_;
        NodePtr     parent_;
        int 		index_;
        std::map<cocos2d::Node*, mjson::Node> configs_;
    };
}

#endif // CMD_DELETE_NODE_H
