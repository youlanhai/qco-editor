#ifndef CMD_CREATE_COMPONENT_H
#define CMD_CREATE_COMPONENT_H

#include "../config.h"
#include <QUndoCommand>
#include <smartjson/src/json.hpp>
#include "../cc_predefine.h"

#include <math/Vec3.h>

namespace Editor
{
    class CmdDeleteComponent;

    class EDITOR_DLL CmdCreateComponent: public QUndoCommand
    {
    public:
        CmdCreateComponent(const mjson::Node &config, NodePtr node, QUndoCommand *parent = 0);

        virtual void undo() override;
        virtual void redo() override;
    private:
        NodePtr         node_;
        ComponentPtr    component_;

        mjson::Node     config_;
        CmdDeleteComponent*  cmdDelete_;
    };
}


#endif // CMD_CREATE_COMPONENT_H
