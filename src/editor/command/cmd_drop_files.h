#ifndef CMD_DROP_FILES_H
#define CMD_DROP_FILES_H


#include "../config.h"
#include <QUndoCommand>
#include <QStringList>
#include <QVector>

#include "../cc_predefine.h"
#include <math/Vec3.h>

namespace Editor
{
    class CmdCreateNode;

    class EDITOR_DLL CmdDropFiles : public QUndoCommand
    {
    public:
        CmdDropFiles(const QStringList &paths, NodePtr nodeParent, const cocos2d::Vec3 &worldPosition, QUndoCommand *parent = 0);

    private:
        void createNodeByDrop(const std::string &path, const cocos2d::Vec3 &position, cocos2d::Node *parent);
    };
}


#endif // CMD_DROP_FILES_H
