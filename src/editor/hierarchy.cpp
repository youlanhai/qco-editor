#include "hierarchy.h"

#include <QTreeWidgetItem>
#include <QMenu>
#include <QAction>

#include <2d/CCNode.h>

#include "runtime/object_loader.h"
#include "tools/log_tool.h"
#include "tools/node_tool.h"
#include "framework.h"
#include "qhierarchytree.h"
#include "menu_loader.h"
#include "document.h"

#include "canvas/editor_multi_selector.h"

#include "command/cmd_create_node.h"
#include "command/cmd_delete_node.h"
#include "command/cmd_property_change.h"
#include "command/cmd_reorder_node.h"
#include "command/cmd_reparent_node.h"
#include "command/cmd_drop_files.h"

#include "properties/property_param.h"

namespace Editor
{
    namespace
    {
        QString name2title(const std::string &name)
        {
            return QString(name.empty() ? "no-name" : name.c_str());
        }

        const int UserRoleIndex = Qt::UserRole + 1;

        cocos2d::Node* qvariant2node(const QVariant &value)
        {
            return reinterpret_cast<cocos2d::Node*>(value.value<quintptr>());
        }

        QVariant node2qvariant(cocos2d::Node *node)
        {
            return QVariant::fromValue(reinterpret_cast<quintptr>(node));
        }

        cocos2d::Node* treeitem2node(QTreeWidgetItem *item)
        {
            return qvariant2node(item->data(0, UserRoleIndex));
        }
    }

    Hierarchy::Hierarchy(QObject *parent, QHierarchyTree *treeView)
        : QObject(parent)
        , treeView_(treeView)
        , contextMenu_(nullptr)
    {
        //connect(treeView_, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SLOT(onCurrentItemChange(QTreeWidgetItem*,QTreeWidgetItem*)));
        connect(treeView_, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChange()));
        connect(treeView_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));
        connect(treeView_, SIGNAL(signalItemDropped(QList<QTreeWidgetItem*>)), this, SLOT(onItemDropped(QList<QTreeWidgetItem*>)));
        connect(treeView_, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(onItemChanged(QTreeWidgetItem*,int)));
        connect(treeView_, SIGNAL(signalFileDropped(QTreeWidgetItem*,QStringList)), this, SLOT(onFileDropped(QTreeWidgetItem*,QStringList)));

        contextMenu_ = new QMenu(treeView_);
        menuCreate_ = contextMenu_->addMenu(tr("new"));
        actionRename_ = contextMenu_->addAction(tr("rename"), this, SLOT(onActionRename()));
        actionRefresh_ = contextMenu_->addAction(tr("refresh"), this, SLOT(onActionRefresh()));
        actionRemove_ = contextMenu_->addAction(tr("remove"), this, SLOT(onActionRemove()));
        actionDuplicate_ = contextMenu_->addAction(tr("dumplicate"), this, SLOT(onActionDuplicate()));

        menuMove_ = contextMenu_->addMenu(tr("move"));
        menuMove_->addAction(tr("move up"), this, SLOT(onActionMoveUp()));
        menuMove_->addAction(tr("move down"), this, SLOT(onActionMoveDown()));

        objectMenuLoader_ = new MenuLoader(this, menuCreate_);
        connect(objectMenuLoader_, SIGNAL(signalCreateObject(JsonValue)), this, SLOT(onActionCreateObject(JsonValue)));
    }

    void Hierarchy::onRootSet(cocos2d::Node *root)
    {
        rootNode_ = root;

        treeView_->clear();
        node2item_.clear();

        if(rootNode_)
        {
            QTreeWidgetItem *item = loadNodeItem(root);
            if(item != nullptr)
            {
                treeView_->addTopLevelItem(item);
                item->setExpanded(true);
            }
        }
    }

    void Hierarchy::onTargetSet(cocos2d::Node *target)
    {
        targetNode_ = target;
        if(target)
        {
            auto it = node2item_.find(target);
            if(it == node2item_.end())
            {
                return;
            }

            if(treeView_->currentItem() != it->second)
            {
                treeView_->setCurrentItem(it->second);
            }
        }
        else
        {
            treeView_->setCurrentItem(nullptr);
        }
    }

    void Hierarchy::onNodeCreate(cocos2d::Node *node)
    {
        if(node == rootNode_)
        {
            return;
        }

        QTreeWidgetItem *parentItem = findParentItem(node);
        CCAssert(parentItem != nullptr, "Hierarchy::onNodeCreate: parent must be in cache.");

        QTreeWidgetItem *item = loadNodeItem(node);
        if(item != nullptr)
        {
            parentItem->addChild(item);

            item->setExpanded(true);
            treeView_->setCurrentItem(item);
        }
    }

    void Hierarchy::onNodeDelete(cocos2d::Node *node)
    {
        auto it = node2item_.find(node);
        if(it == node2item_.end())
        {
            return;
        }
        QTreeWidgetItem *item = it->second;
        node2item_.erase(it);

        // remove child item rescursively
        auto &children = node->getChildren();
        for(cocos2d::Node *child : children)
        {
            onNodeDelete(child);
        }

        delete item;
    }

    void Hierarchy::onPropertyChange(PropertyParam &param)
    {
        cocos2d::Node *node = static_cast<cocos2d::Node*>(param.object);

        if(param.name == "name")
        {
            auto it = node2item_.find(node);
            if(it != node2item_.end())
            {
                QTreeWidgetItem *item = it->second;
                item->setText(0, name2title(node->getName()));
            }
        }
        else if(param.name == "order")
        {
            sortNodeChildren(node->getParent());
        }
    }

    QTreeWidgetItem* Hierarchy::loadNodeItem(cocos2d::Node *node)
    {
        if(!Framework::instance()->existNode(node))
        {
            return nullptr;
        }

        QTreeWidgetItem *item = new QTreeWidgetItem((QTreeWidget*)nullptr);
        item->setText(0, name2title(node->getName()));
        item->setData(0, UserRoleIndex, node2qvariant(node));
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        node2item_[node] = item;

        auto & children = node->getChildren();
        for(auto child : children)
        {
            QTreeWidgetItem *subItem = loadNodeItem(child);
            if(subItem != nullptr)
            {
                item->addChild(subItem);
            }
        }

        return item;
    }
    
    QTreeWidgetItem* Hierarchy::findParentItem(cocos2d::Node *node)
    {
        //结点的父不一定在池子中。例如ListView，其child都是引用自内部容器的。
        cocos2d::Node *parent = node->getParent();
        if(parent != nullptr)
        {
            auto it = node2item_.find(parent);
            if(it != node2item_.end())
            {
                return it->second;
            }
            parent = node->getParent();
        }
        return nullptr;
    }

    void Hierarchy::sortNodeChildren(cocos2d::Node *node)
    {
        //结点的父不一定在池子中。例如ListView，其child都是引用自内部容器的。
        NodeItemMap::iterator it;
        while(node != nullptr)
        {
            it = node2item_.find(node);
            if(it != node2item_.end())
            {
                break;
            }
            node = node->getParent();
        }
        if(node == nullptr)
        {
            return;
        }
        
        node->sortAllChildren();

        QTreeWidgetItem *item = it->second;

        // remove all children
        item->takeChildren();

        // then add child again.
        QList<QTreeWidgetItem*> subItems;
        auto & children = node->getChildren();
        for(auto child : children)
        {
            auto iter = node2item_.find(child);
            if(iter != node2item_.end())
            {
                QTreeWidgetItem *subItem = iter->second;
                subItems.append(subItem);
            }
        }

        if(!subItems.empty())
        {
            item->addChildren(subItems);
            item->setExpanded(true);
        }
    }

    void Hierarchy::selectNode(cocos2d::Node *node)
    {
        QTreeWidgetItem *item = nullptr;

        if(node != nullptr)
        {
            auto it = node2item_.find(node);
            if(it != node2item_.end())
            {
                item = it->second;
            }
        }

        treeView_->setCurrentItem(item);
    }

    void Hierarchy::onCurrentItemChange(QTreeWidgetItem *current, QTreeWidgetItem * /*previous*/)
    {
        if(current != nullptr)
        {
            cocos2d::Node *node = treeitem2node(current);
            if(node != targetNode_)
            {
                emit signalSetTarget(node);
            }
        }
    }

    void Hierarchy::onSelectionChange()
    {
        QList<QTreeWidgetItem*> items = treeView_->selectedItems();
        if(items.empty())
        {
            if(targetNode_ != nullptr)
            {
                emit signalSetTarget(nullptr);
            }
        }
        else if(items.size() == 1)
        {
            cocos2d::Node *node = treeitem2node(items.first());
            if(node != targetNode_)
            {
                emit signalSetTarget(node);
            }
        }
        else
        {
            auto selector = Framework::instance()->document_->getMultiSelector();

            std::vector<cocos2d::Node*> nodes;
            foreach(QTreeWidgetItem* item, items)
            {
                nodes.push_back(treeitem2node(item));
            }
            selector->setSelection(nodes);

            emit signalSetTarget(selector);
        }
    }

    void Hierarchy::onItemDropped(const QList<QTreeWidgetItem*> &items)
    {
        QVector<QUndoCommand*> commands;

        foreach(QTreeWidgetItem *item, items)
        {
            QTreeWidgetItem *parentItem = item->parent();
            if(nullptr == parentItem)
            {
                continue;
            }
            cocos2d::Node *parentNode = treeitem2node(parentItem);
            CCAssert(parentNode != nullptr, "onItemDropped: the parent node must not be null.");

            cocos2d::Node *node = treeitem2node(item);
            int newIndex = parentItem->indexOfChild(item);
            if(!parentNode->getChildren().contains(node))
            {
                commands.push_back(new CmdReparentNode(node, parentNode, newIndex));
            }
            else if(nodeGetIndex(node) != newIndex)
            {
                commands.push_back(new CmdReorderNode(node, newIndex));
            }
        }

        foreach(QUndoCommand *cmd, commands)
        {
            Framework::instance()->pushCommand(cmd);
        }

        if(!items.empty())
        {
            foreach(QTreeWidgetItem *item, items)
            {
                item->setSelected(true);
            }

            treeView_->setCurrentItem(items.first());
        }
    }

    void Hierarchy::onContextMenuRequested(const QPoint &pos)
    {
        bool hasItem = treeView_->currentItem() != nullptr;

        menuMove_->setEnabled(hasItem);
        actionRefresh_->setEnabled(hasItem);
        actionRename_->setEnabled(hasItem);
        actionRemove_->setEnabled(hasItem);

        contextMenu_->exec(treeView_->mapToGlobal(pos));
    }

    void Hierarchy::onActionRefresh()
    {
        onRootSet(rootNode_);
    }

    void Hierarchy::onActionRename()
    {
        if(treeView_->currentItem() != nullptr)
        {
            treeView_->editItem(treeView_->currentItem());
        }
    }

    void Hierarchy::onActionRemove()
    {
        QList<QTreeWidgetItem*> items = treeView_->selectedItems();
        foreach(QTreeWidgetItem *item, items)
        {
            cocos2d::Node *node = treeitem2node(item);
            Framework::instance()->pushCommand(new CmdDeleteNode(node));
        }
    }

    void Hierarchy::onActionMoveUp()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(nullptr == item || item->parent() == nullptr)
        {
            return;
        }

        cocos2d::Node *node = treeitem2node(item);
        int index = nodeGetIndex(node);
        if(index <= 0)
        {
            return;
        }

        Framework::instance()->pushCommand(new CmdReorderNode(node, index - 1));
    }

    void Hierarchy::onActionMoveDown()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        if(nullptr == item || item->parent() == nullptr)
        {
            return;
        }

        cocos2d::Node *node = treeitem2node(item);
        int index = nodeGetIndex(node);
        int lastIndex = node->getParent()->getChildrenCount() - 1;
        if(index < 0 || index >= lastIndex)
        {
            return;
        }

        Framework::instance()->pushCommand(new CmdReorderNode(node, index + 1));
    }

    void Hierarchy::onItemChanged(QTreeWidgetItem *item, int /*column*/)
    {
        std::string name = item->text(0).toUtf8().data();
        LOG_DEBUG("item changed: %s", name.c_str());

        cocos2d::Node *node = treeitem2node(item);
        if(node != nullptr && node->getName() != name)
        {
            Framework::instance()->pushCommand(new CmdPropertyChange(node, "name", name));
        }
    }

    bool Hierarchy::loadNewMenu(const std::string &filename)
    {
        return objectMenuLoader_->loadMenu(filename);
    }

    void Hierarchy::onActionCreateObject(const JsonValue &config)
    {
        QTreeWidgetItem *item = treeView_->currentItem();

        cocos2d::Node *parent = nullptr;
        if(item != nullptr)
        {
            parent = treeitem2node(item);
        }

        Framework::instance()->pushCommand(new CmdCreateNode(config, parent));
    }

    void Hierarchy::onActionDuplicate()
    {
        QTreeWidgetItem *item = treeView_->currentItem();
        cocos2d::Node *node = treeitem2node(item);

        mjson::Node config;
        if(!Framework::instance()->document_->saveNodeToJson(node, config))
        {
            return;
        }

        QTreeWidgetItem *parentItem = item->parent();
        cocos2d::Node *parent = nullptr;
        if(nullptr != parentItem)
        {
            parent = treeitem2node(parentItem);
        }

        EDITOR_LOCK_GL_SCOPE();
        Framework::instance()->pushCommand(new CmdCreateNode(config, parent));
    }

    void Hierarchy::onFileDropped(QTreeWidgetItem *item, const QStringList &paths)
    {
        cocos2d::Node *parent = treeitem2node(item);
        cocos2d::Vec3 worldPosition = cocos2d::Vec3::ZERO;
        if(parent != nullptr)
        {
            parent->getNodeToWorldTransform().transformPoint(&worldPosition);
        }
        
        EDITOR_LOCK_GL_SCOPE();
        Framework::instance()->pushCommand(new CmdDropFiles(paths, parent, worldPosition));
    }

} // end namespace Editor
