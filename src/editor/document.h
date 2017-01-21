#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "config.h"
#include <QObject>
#include <QUndoStack>
#include <smartjson/src/json.hpp>
#include "cc_predefine.h"
#include "common_types.h"

class JsonHandle;
typedef mjson::Node         JsonValue;

class ObjectLoader;

namespace Editor
{
    class EditorMultiSelector;

    class EDITOR_DLL Document : public QObject
    {
        Q_OBJECT
    public:

        static Document* create(QObject *parent, SceneType type);
        static Document* create(QObject *parent, const QString &fileName);

        Document(QObject *parent);
        ~Document();

        bool loadFromFile(const QString &fileName);
        bool saveToFile(const QString &fileName);

        void attachToScene(cocos2d::Node *sceneRoot);

        const QString& getFileName() const { return fileName_; }

        cocos2d::Node* createNode(const JsonValue &property);
        void addNode(cocos2d::Node *node, const JsonValue &property);
        void removeNode(cocos2d::Node *node);

        cocos2d::Component* createComponent(cocos2d::Node *node, const JsonHandle &config, std::string name = "");
        bool renameComponent(cocos2d::Component *component, const std::string &newName);

        JsonValue findNodeConfig(cocos2d::Node *node);
        bool saveNodeToJson(cocos2d::Node *node, JsonValue &out);

        void setNodeConfig(cocos2d::Node *node, const std::string &name, const JsonValue &value);

        JsonValue findComponentConfig(cocos2d::Component *com);

        void setSceneType(SceneType type){ type_ = type; }
        SceneType getSceneType() const { return type_; }

        void setRootNode(NodePtr node);
        NodePtr getRootNode(){ return rootNode_; }

        void setTargetNode(NodePtr node);
        NodePtr getTargetNode(){ return targetNode_; }

        QUndoStack& getUndoStack(){ return undoStack_; }

        cocos2d::RefPtr<EditorMultiSelector> getMultiSelector(){ return multiSelector_; }

        int genNameSerialNumber();
        std::string genNodeName(const std::string &prefix);

    public slots:

        void slotRenameFile(const QString &oldName, const QString &newName);

    protected:
        bool createMultiSelector();

        cocos2d::Node* loadNodeFromStream(const JsonHandle & config);
        void loadNodeProperty(const JsonHandle &config, ObjectLoader *loader, cocos2d::Node *p);
        void loadNodeComponents(const JsonHandle &config, cocos2d::Node *p);
        void loadNodeChildren(const JsonHandle &config, cocos2d::Node *p);

        SceneType           type_;
        NodePtr             rootNode_;
        NodePtr             targetNode_;
        QString             fileName_;

        cocos2d::RefPtr<EditorMultiSelector> multiSelector_;

        typedef std::map<cocos2d::Node*, JsonValue> ConfigureMap;
        ConfigureMap        configures_;

        QUndoStack          undoStack_;
        int                 nameSerialNumber_;
    };

} // end namespace Editor

#endif // DOCUMENT_H
