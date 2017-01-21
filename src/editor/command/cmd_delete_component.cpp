#include "cmd_delete_component.h"
#include "../framework.h"
#include "../document.h"
#include "../tools/node_tool.h"
#include <2d/CCNode.h>
#include <2d/CCComponent.h>

namespace Editor
{
    CmdDeleteComponent::CmdDeleteComponent(ComponentPtr component, QUndoCommand *parent)
        : QUndoCommand(parent)
        , node_(component->getOwner())
        , component_(component)
    {
        config_ = Framework::instance()->document_->findComponentConfig(component);
    }

    void CmdDeleteComponent::undo()
    {

    }

    void CmdDeleteComponent::redo()
    {
//        Framework::instance()->removeComponent(node_);
    }
}
