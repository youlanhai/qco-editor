#ifndef CMD_PROPERTY_CHANGE_H
#define CMD_PROPERTY_CHANGE_H

#include "../config.h"
#include <QUndoCommand>
#include "../cc_predefine.h"
#include <smartjson/src/json.hpp>

namespace Editor
{
    class EDITOR_DLL CmdPropertyChange : public QUndoCommand
    {
    public:
        CmdPropertyChange(NodePtr node, const std::string &key, const mjson::Node &value, QUndoCommand *parent = 0);

        virtual int	id() const override;
        virtual bool mergeWith(const QUndoCommand * command) override;
        virtual void undo() override;
        virtual void redo() override;

    private:
        NodePtr         node_;
        std::string     key_;
        mjson::Node     value_;
        mjson::Node     oldValue_;
    };
}

#endif // CMD_PROPERTY_CHANGE_H
