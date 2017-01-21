#ifndef CMD_CREATE_NODE_H
#define CMD_CREATE_NODE_H

#include "../config.h"
#include <QUndoCommand>
#include <smartjson/src/json.hpp>
#include "../cc_predefine.h"

#include <math/Vec3.h>

namespace Editor
{
    class CmdDeleteNode;

    class EDITOR_DLL CmdCreateNode : public QUndoCommand
    {
    public:
        CmdCreateNode(const mjson::Node &config, NodePtr nodeParent, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;
    private:
        NodePtr         node_;
        NodePtr         parent_;
        cocos2d::Vec3   worldPosition_;
        mjson::Node     config_;
        CmdDeleteNode*  cmdDelete_;
    };
}

#endif // CMD_CREATE_NODE_H
