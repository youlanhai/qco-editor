#include "cmd_create_component.h"
#include "../framework.h"
#include "../document.h"
#include "cmd_delete_component.h"
#include <2d/CCNode.h>
#include <2d/CCComponent.h>

namespace Editor
{
    CmdCreateComponent::CmdCreateComponent(const mjson::Node &config, NodePtr node, QUndoCommand *parent)
        : QUndoCommand(parent)
        , config_(config)
        , node_(node)
        , cmdDelete_(nullptr)
    {

    }

    void CmdCreateComponent::undo()
    {
        if(cmdDelete_ != nullptr)
        {
            cmdDelete_->redo();
        }
    }

    void CmdCreateComponent::redo()
    {
        if(cmdDelete_ != nullptr)
        {
            cmdDelete_->undo();
        }
        else
        {
            component_ = Framework::instance()->document_->createComponent(node_, config_);
            if(component_)
            {
                cmdDelete_ = new CmdDeleteComponent(component_);
            }
        }
    }
}
