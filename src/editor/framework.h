#ifndef EDITOR_H
#define EDITOR_H

#include <QObject>

#include "config.h"
#include "tools/json_tools.h"
#include "common_types.h"
#include "cc_predefine.h"
#include "runtime/singleton.h"

// predefination
class PropertyParam;
class QUndoCommand;

NS_CC_BEGIN
class Component;
NS_CC_END

namespace Editor
{
    class Hierarchy;
    class CanvasMgr;
    class Inspector;
    class MenuLoader;
    class Document;
    class ResourceMgr;
    class ResourceFolder;
    class ResourceContent;
    class Importer;
    class LogOut;
    class PluginMgr;
    class StatusBar;

    class EDITOR_DLL Framework : public QObject, public Singleton<Framework>
    {
        Q_OBJECT
    public:
        explicit Framework(QObject *parent = 0);
        ~Framework();

        bool init();

        cocos2d::Node* createNode(const JsonValue &property, cocos2d::Node *parent = nullptr);
        void addNodePrivate(cocos2d::Node *node, cocos2d::Node *parent = nullptr, int index = -1);
        void removeNode(cocos2d::Node *node);
        bool existNode(cocos2d::Node *node);

        void newLayout(SceneType type);
        void closeLayout();
        bool loadLayout(const QString &fileName, bool force = false);
        bool saveLayout(const QString &fileName);
        bool reloadCurrentLayout();

        ScenePtr getScene(){ return scene_; }
        NodePtr getSceneRoot(){ return sceneRoot_; }
        NodePtr getRootNode(){ return rootNode_; }
        NodePtr getTargetNode(){ return targetNode_; }
        JsonValue getTargetConfig(){ return targetConfig_; }

        const QString& getResourcePath() const { return resourcePath_; }
        bool setResourcePath(const QString &resourcePath);

        void emitTargetPropertyChange(const std::string &name, const JsonValue &value);
        void emitNodePropertyChange(cocos2d::Node *node, const std::string &name, const JsonValue &value);

        template<typename T>
        void emitNodePropertyChange(cocos2d::Node *node, const std::string &name, const T &value);

        void emitComponentPropertyChange(cocos2d::Node *node, const std::string &comName, const std::string &propName, const JsonValue &value);
        void emitComponentPropertyChange(cocos2d::Component *component, const std::string &propName, const JsonValue &value);

        void emitObjectPropertyChange(cocos2d::Ref *object, const std::string &name, const JsonValue &value);

        void lockGL();
        void unlockGL();

        void setModified(bool modify);
        bool isModified() const { return isModified_; }

        void refreshTitle();

        bool checkCurrentLayoutModified();
        bool checkCurrentLayoutExist(bool echo = true);

        void pushCommand(QUndoCommand *cmd);

    private:
        void createScene();
        bool loadEditorShader();

        void destroyLayout();
        void setRootNode(cocos2d::Node *root);

        bool createInspectorComponent();
        bool createHierarchyComponent();
        bool createObjectMenuComponent();
        bool createComponentMenuComponent();
        bool createCanvasComponent();
        bool createResourceComponent();
        bool createResourceFolderComponent();
        bool createResourceContentComponent();
        bool createImporterComponent();
        bool createLogOutComponent();
        bool createPluginComponent();
        bool createStatusBarComponent();

    signals:
        void signalRootSet(cocos2d::Node *root);
        void signalTargetSet(cocos2d::Node *target);
        void signalNodeCreate(cocos2d::Node *node);
        void signalNodeDelete(cocos2d::Node *node);

        void signalComponentCreated(const std::string &name);
        void signalComponentRename(cocos2d::Component *p, const std::string &oldName);

        void signalPropertyChange(PropertyParam &param);
        void signalComponentPropertyChange(PropertyParam &param);

    public slots:
        void setTargetNode(cocos2d::Node *target);
        void slotCreateNode(const JsonValue &config);
        void slotCreateComponent(const JsonValue &config);

    public: // public component
        Hierarchy*      hierarchy_;
        CanvasMgr*      canvasMgr_;
        Inspector*      inspector_;
        MenuLoader*     objectMenu_;
        MenuLoader*     componentMenu_;
        Document*       document_;
        ResourceMgr*    resource_;
        ResourceFolder* resourceFolder_;
        ResourceContent* resourceContent_;
        Importer*       importer_;
        LogOut*         logout_;
        PluginMgr*      plugin_;
        StatusBar*		statusBar_;

    private:
        /** cocos2d-x场景结点*/
        ScenePtr        scene_;
        /** 场景下直属的root结点，该结点会接受鼠标缩放操作，
         *  rootNode会挂接在此结点上，其他需要缩放的结点，也挂在此结点上。
         */
        NodePtr 		sceneRoot_;

        /** layout的root结点*/
        NodePtr         rootNode_;
        /** 当前选中的结点*/
        NodePtr         targetNode_;

        JsonValue       targetConfig_;

        QString         resourcePath_;

        bool            isModified_;
    };

    class EDITOR_DLL GlLockHolder 
    {
    public:
        GlLockHolder();
        ~GlLockHolder();
    };

#define EDITOR_LOCK_GL_SCOPE() ::Editor::GlLockHolder _gl_locker

    template<typename T>
    void Framework::emitNodePropertyChange(cocos2d::Node *node, const std::string &name, const T &value)
    {
        if (document_ != nullptr)
        {
            JsonValue jvalue = jsonFrom(value);
            this->emitNodePropertyChange(node, name, jvalue);
        }
    }

}// end namespace Editor

#endif // EDITOR_H
