#ifndef EDITOR_HIERARCHY_H
#define EDITOR_HIERARCHY_H

#include "config.h"
#include <QObject>
#include <QModelIndex>
#include <base/CCRefPtr.h>
#include <map>
#include <smartjson/src/json.hpp>

NS_CC_BEGIN
class Node;
NS_CC_END

class QHierarchyTree;
class QTreeWidgetItem;
class QMenu;
class QAction;

class PropertyParam;
typedef mjson::Node JsonValue;

namespace Editor
{
    typedef cocos2d::RefPtr<cocos2d::Node> NodePtr;
    class MenuLoader;

    class EDITOR_DLL Hierarchy : public QObject
    {
        Q_OBJECT
    public:
        explicit Hierarchy(QObject *parent, QHierarchyTree *treeView);

        bool loadNewMenu(const std::string &filename);
        void sortNodeChildren(cocos2d::Node *node);

        void selectNode(cocos2d::Node *node);

    signals:
        void signalSetTarget(cocos2d::Node *target);

    public slots:
        void onRootSet(cocos2d::Node *root);
        void onTargetSet(cocos2d::Node *target);
        void onNodeCreate(cocos2d::Node *node);
        void onNodeDelete(cocos2d::Node *node);
        void onPropertyChange(PropertyParam &param);

        void onContextMenuRequested(const QPoint &pos);

        void onActionRefresh();
        void onActionRename();
        void onActionRemove();
        void onActionMoveUp();
        void onActionMoveDown();
        void onActionDuplicate();

        void onCurrentItemChange(QTreeWidgetItem *current, QTreeWidgetItem *previous);
        void onSelectionChange();
        void onItemDropped(const QList<QTreeWidgetItem *> &items);
        void onItemChanged(QTreeWidgetItem *item, int column);

        void onActionCreateObject(const JsonValue &config);

        void onFileDropped(QTreeWidgetItem *item, const QStringList &paths);

    private:
        QTreeWidgetItem* loadNodeItem(cocos2d::Node *node);
        QTreeWidgetItem* findParentItem(cocos2d::Node *node);

        NodePtr         rootNode_;
        NodePtr         targetNode_;

        QHierarchyTree* treeView_;

        QMenu*          contextMenu_;
        MenuLoader*     objectMenuLoader_;

        QMenu*          menuCreate_;
        QMenu*          menuMove_;
        QAction*        actionRename_;
        QAction*        actionRefresh_;
        QAction*        actionRemove_;
        QAction*        actionDuplicate_;

        typedef std::map<cocos2d::Node*, QTreeWidgetItem*> NodeItemMap;
        NodeItemMap     node2item_;
    };

} // end namespace Editor

#endif // EDITOR_HIERARCHY_H
