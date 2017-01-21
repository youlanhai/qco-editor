#include "framework.h"

#include "properties/property_creator.h"
#include "properties/property_group.h"
#include "properties/property_default.h"
#include "properties/property_param.h"
#include "properties/register_property_listeners.h"

#include "hierarchy.h"
#include "canvas/canvas_mgr.h"
#include "inspector.h"
#include "menu_loader.h"
#include "document.h"
#include "resource_mgr.h"
#include "resource_folder.h"
#include "resource_content.h"
#include "configure_loader.h"
#include "project_mgr.h"
#include "resource.h"
#include "importer.h"
#include "logout.h"
#include "plugin_mgr.h"
#include "status_bar.h"
#include "user_configure.h"
#include "class_name_map.h"

#include "canvas/editor_multi_selector.h"
#include "canvas/canvas_creator.h"

#include "tools/json_tools.h"
#include "tools/log_tool.h"
#include "tools/file_tools.h"
#include "tools/node_tool.h"
#include "tools/qstring_tool.h"

#include "command/cmd_create_node.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <runtime/object_factory.h>
#include <runtime/layout_loader_mgr.h>
#include <runtime/component_loader_mgr.h>
#include <runtime/loaders/loader_addin.h>

#include <base/CCDirector.h>
#include <2d/CCNode.h>
#include <2d/CCCamera.h>
#include <physics3d/CCPhysics3D.h>
#include <physics/CCPhysicsWorld.h>
#include <renderer/CCGLProgram.h>
#include <renderer/CCGLProgramCache.h>
#include <platform/CCFileUtils.h>

#include <QMessageBox>

IMPLEMENT_SINGLETON(Editor::Framework);


#define LOAD_EDITOR_COMPONENT(NAME) \
if(!create##NAME##Component()) \
{\
    LOG_ERROR("Failed to load component '%s'", #NAME);\
    return false;\
}

static bool loadShader(const std::string &name)
{
    cocos2d::GLProgram *shader = cocos2d::GLProgram::createWithFilenames(name + ".vert", name + ".frag");
    if(shader == nullptr)
    {
        LOG_ERROR("Failed to load shader '%s'", name.c_str());
        return false;
    }

    cocos2d::GLProgramCache::getInstance()->addGLProgram(shader, name);
    LOG_DEBUG("Load shader '%s'", name.c_str());
    return true;
}

namespace Editor
{
    Framework::Framework(QObject *parent)
        : QObject(parent)
        , hierarchy_(nullptr)
        , canvasMgr_(nullptr)
        , inspector_(nullptr)
        , objectMenu_(nullptr)
        , componentMenu_(nullptr)
        , document_(nullptr)
        , resource_(nullptr)
        , resourceFolder_(nullptr)
        , resourceContent_(nullptr)
        , importer_(nullptr)
        , logout_(nullptr)
        , plugin_(nullptr)
        , statusBar_(nullptr)
        , isModified_(false)
    {
        LayoutLoaderMgr::initInstance();
        ComponentLoaderMgr::initInstance();

        PropertyCreatorMgr::initInstance();
        PropertyGroupMgr::initInstance();
        PropertyDefault::initInstance();
        ConfigureLoader::initInstance();
        UserConfigure::initInstance();
        CanvasFactory::initInstance();

        ObjectFactory::instance()->registerLoader("EditorMultiSelector", new EditorMultiSelectorLoader());
    }

    Framework::~Framework()
    {
        PropertyGroupMgr::finiInstance();
        PropertyCreatorMgr::finiInstance();
        PropertyDefault::finiInstance();
        ConfigureLoader::finiInstance();
        UserConfigure::finiInstance();
        CanvasFactory::finiInstance();

        LayoutLoaderMgr::finiInstance();
        ComponentLoaderMgr::finiInstance();
    }

    bool Framework::init()
    {
        std::string editorResPath = ProjectManager::instance()->getEditorResourcePath().toStdString();
        cocos2d::FileUtils::getInstance()->addSearchPath(editorResPath, true);

        createScene();
        if(!loadEditorShader())
        {
            return false;
        }

        // 初始化编辑器组件
        LOAD_EDITOR_COMPONENT(Inspector);
        LOAD_EDITOR_COMPONENT(Hierarchy);
        LOAD_EDITOR_COMPONENT(Canvas);
        LOAD_EDITOR_COMPONENT(ObjectMenu);
        LOAD_EDITOR_COMPONENT(ComponentMenu);
        LOAD_EDITOR_COMPONENT(Resource);
        LOAD_EDITOR_COMPONENT(ResourceFolder);
        LOAD_EDITOR_COMPONENT(ResourceContent);
        LOAD_EDITOR_COMPONENT(Importer);
        LOAD_EDITOR_COMPONENT(LogOut);
        LOAD_EDITOR_COMPONENT(Plugin);
        LOAD_EDITOR_COMPONENT(StatusBar);

        // 连接消息监听
        connect(this, SIGNAL(signalTargetSet(cocos2d::Node*)),
                MainWindow::instance(), SLOT(onTargetSet(cocos2d::Node*)));

        connect(this, SIGNAL(signalPropertyChange(PropertyParam&)),
                inspector_, SLOT(onPropertyChange(PropertyParam&)));
        connect(this, SIGNAL(signalComponentPropertyChange(PropertyParam&)),
                inspector_, SLOT(onComponentPropertyChange(PropertyParam&)));
        connect(this, SIGNAL(signalComponentRename(cocos2d::Component*,std::string)),
                inspector_, SLOT(onComponentRename(cocos2d::Component*,std::string)));

        connect(this, SIGNAL(signalPropertyChange(PropertyParam&)),
                canvasMgr_, SLOT(onPropertyChange(PropertyParam&)));
        connect(this, SIGNAL(signalComponentPropertyChange(PropertyParam&)),
                canvasMgr_, SLOT(onComponentPropertyChange(PropertyParam&)));

        connect(this, SIGNAL(signalPropertyChange(PropertyParam&)),
                hierarchy_, SLOT(onPropertyChange(PropertyParam&)));

        // 初始化工程的资源路径
        if(!setResourcePath(ProjectManager::instance()->getResourcePath()))
        {
            LOG_ERROR("Failed to load resource '%s'", resourcePath_.toUtf8().data());
            return false;
        }


        registerLoaders(ObjectFactory::instance());
        registerClassNameMap(PropertyGroupMgr::instance());



        if(!ConfigureLoader::instance()->addConfigure(Editor::Res::PropertyConf))
        {
            return false;
        }

        registerPropertyListeners();

        // 加载插件
        if(!plugin_->loadPlugins())
        {
            LOG_ERROR("Failed to load plugins.");
            return false;
        }

        // 加载工程配置文件
        QString projectConfigure = joinPath(ProjectManager::instance()->getProjectPath(), QString("plugins/configure.json"));
        if(!ConfigureLoader::instance()->tryAddConfigure(projectConfigure))
        {
            return false;
        }

        // 加载用户配置文件
        QString userConfigure = joinPath(ProjectManager::instance()->getProjectPath(), USER_CONFIG_FILE_NAME);
        UserConfigure::instance()->load(userConfigure);

        newLayout(SceneType::TP_2D);
        return true;
    }

    void Framework::createScene()
    {
        scene_ = cocos2d::Scene::createWithPhysics();

        // disable physics simulation.
        scene_->getPhysicsWorld()->setAutoStep(false);

        sceneRoot_ = cocos2d::Node::create();
        scene_->addChild(sceneRoot_);

        cocos2d::Director::getInstance()->replaceScene(scene_);
    }

    bool Framework::loadEditorShader()
    {
        // 加载编辑器用到的shader
        // TODO 做成配置文件
        std::vector<std::string> shaderNames;
        shaderNames.push_back(Res::GridShader);
        for(const std::string &name : shaderNames)
        {
            if(!loadShader(name))
            {
                return false;
            }
        }
        return true;
    }

    void Framework::setRootNode(cocos2d::Node *root)
    {
        if(rootNode_ == root)
        {
            return;
        }
        if(rootNode_)
        {
            rootNode_->removeFromParent();
        }

        rootNode_ = root;
        document_->setRootNode(root);
        emit signalRootSet(root);
    }

    void Framework::setTargetNode(cocos2d::Node *target)
    {
        if(targetNode_ == target)
        {
            return;
        }

        if(target != nullptr)
        {
            // search target configure.
            JsonValue config = document_->findNodeConfig(target);
            if(config.isNull())
            {
                target = nullptr;
                LOG_ERROR("The target wasn't created by Editor.");
            }

            targetConfig_ = config;
        }
        targetNode_ = target;
        document_->setTargetNode(target);

        emit signalTargetSet(target);
    }

    void Framework::emitTargetPropertyChange(const std::string &name, const JsonValue &value)
    {
        emitNodePropertyChange(targetNode_, name, value);
    }

    void Framework::emitNodePropertyChange(cocos2d::Node *node, const std::string &name, const JsonValue &value)
    {
        if(nullptr == document_)
        {
            return;
        }

        JsonValue config = document_->findNodeConfig(node);
        if(!config.isDict())
        {
            return;
        }

        JsonValue oldValue = config.find(name.c_str());
        if(value == oldValue)
        {
            return;
        }
        config[name] = value;
        setModified(true);

        PropertyParam param(node, name, value, config);
        emit signalPropertyChange(param);
    }

    void Framework::emitComponentPropertyChange(cocos2d::Node *node, const std::string &comName, const std::string &propName, const JsonValue &value)
    {
        cocos2d::Component *component = node->getComponent(comName);
        if(nullptr == component)
        {
            return;
        }

        emitComponentPropertyChange(component, propName, value);
    }

    void Framework::emitComponentPropertyChange(cocos2d::Component *component, const std::string &propName, const JsonValue &value)
    {
        if(nullptr == document_)
        {
            return;
        }

        JsonHandle nodeConfig = document_->findNodeConfig(component->getOwner());
        JsonHandle config = nodeConfig["components"][component->getName().c_str()];
        if(!config.isDict())
        {
            return;
        }

        config.setMember(propName.c_str(), value);
        setModified(true);

        PropertyParam param(component, propName, value, config);
        emit signalComponentPropertyChange(param);
    }

    void Framework::emitObjectPropertyChange(cocos2d::Ref *object, const std::string &name, const JsonValue &value)
    {
        cocos2d::Node *node = dynamic_cast<cocos2d::Node*>(object);
        if(node != nullptr)
        {
            emitNodePropertyChange(node, name, value);
            return;
        }

        cocos2d::Component *component = dynamic_cast<cocos2d::Component*>(object);
        if(component != nullptr)
        {
            emitComponentPropertyChange(component, name, value);
            return;
        }
    }

    cocos2d::Node *Framework::createNode(const JsonValue &property, cocos2d::Node *parent)
    {
        cocos2d::Node *node = document_->createNode(property);
        if(nullptr == node)
        {
            return nullptr;
        }

        addNodePrivate(node, parent);
        return node;
    }

    void Framework::addNodePrivate(cocos2d::Node *node, cocos2d::Node *parent, int index)
    {
        if(!rootNode_)
        {
            sceneRoot_->addChild(node);
            setRootNode(node);
        }
        else if(parent != nullptr)
        {
            parent->addChild(node);
        }
        else
        {
            rootNode_->addChild(node);
        }

        if(index >= 0)
        {
            nodeSetIndex(node, index);
        }

        emit signalNodeCreate(node);
        setTargetNode(node);

        setModified(true);
    }

    void Framework::removeNode(cocos2d::Node *node)
    {
        node->retain();

        document_->removeNode(node);

        if(node == targetNode_)
        {
            setTargetNode(nullptr);
        }

        if(node == rootNode_)
        {
            setRootNode(nullptr);
        }

        emit signalNodeDelete(node);

        node->removeFromParent();
        node->release();

        setModified(true);
    }

    bool Framework::existNode(cocos2d::Node *node)
    {
        return !(document_->findNodeConfig(node).isNull());
    }

    bool Framework::loadLayout(const QString &fileName, bool force)
    {
        if(!force && fileName == document_->getFileName())
        {
            return false;
        }
        
        Document *doc = Document::create(this, fileName);
        if(!doc)
        {
            return false;
        }

        LOG_DEBUG("load layout: %s", fileName.toUtf8().data());
        destroyLayout();

        document_ = doc;
        document_->attachToScene(sceneRoot_);
        canvasMgr_->setCanvasMode((int)document_->getSceneType());

        connect(resource_, SIGNAL(signalRenameFile(QString, QString)), doc, SLOT(slotRenameFile(QString, QString)));

        setRootNode(document_->getRootNode());
        setTargetNode(document_->getRootNode());

        refreshTitle();
        return true;
    }

    bool Framework::saveLayout(const QString &fileName)
    {
        LOG_DEBUG("save layout: %s", fileName.toUtf8().data());
        if(document_->saveToFile(fileName))
        {
            setModified(false);
            return true;
        }
        return false;
    }

    bool Framework::reloadCurrentLayout()
    {
        // make a copy of the name string. destroy operation will also destroy the name string.
        QString originFileName = document_->getFileName();
        return loadLayout(originFileName, true);
    }

    void Framework::destroyLayout()
    {
        if(document_ != nullptr)
        {
            setTargetNode(nullptr);
            setRootNode(nullptr);

            delete document_;
            document_ = nullptr;
        }

        isModified_ = false;
    }

    void Framework::newLayout(SceneType type)
    {
        destroyLayout();
        canvasMgr_->setCanvasMode((int)type);

        document_ = Document::create(this, type);
        document_->attachToScene(sceneRoot_);

        refreshTitle();
    }

    void Framework::closeLayout()
    {
        newLayout(document_->getSceneType());
    }

    bool Framework::createInspectorComponent()
    {
        QDockWidget *widget = MainWindow::instance()->ui->inspectorWidget;

        inspector_ = new Inspector(this, widget);
        connect(this, SIGNAL(signalTargetSet(cocos2d::Node*)), inspector_, SLOT(onTargetSet(cocos2d::Node*)));
        connect(this, SIGNAL(signalComponentCreated(std::string)), inspector_, SLOT(onComponentAdd(std::string)));
        //connect(inspector_, SIGNAL(signalPropertyChange(PropertyParam&)), inspector_, SLOT(onPropertyChange(PropertyParam&)));
        return true;
    }

    bool Framework::createHierarchyComponent()
    {
        hierarchy_ = new Hierarchy(this, MainWindow::instance()->ui->hierarchyTree);
        connect(hierarchy_, SIGNAL(signalSetTarget(cocos2d::Node*)), this, SLOT(setTargetNode(cocos2d::Node*)));
        connect(this, SIGNAL(signalRootSet(cocos2d::Node*)), hierarchy_, SLOT(onRootSet(cocos2d::Node*)));
        connect(this, SIGNAL(signalTargetSet(cocos2d::Node*)), hierarchy_, SLOT(onTargetSet(cocos2d::Node*)));
        connect(this, SIGNAL(signalNodeCreate(cocos2d::Node*)), hierarchy_, SLOT(onNodeCreate(cocos2d::Node*)));
        connect(this, SIGNAL(signalNodeDelete(cocos2d::Node*)), hierarchy_, SLOT(onNodeDelete(cocos2d::Node*)));
        //connect(inspector_, SIGNAL(signalPropertyChange(PropertyParam&)), hierarchy_, SLOT(onPropertyChange(PropertyParam&)));

        return true;
    }

    bool Framework::createObjectMenuComponent()
    {
        QMenu *menu = MainWindow::instance()->ui->menuCreate;
        objectMenu_ = new MenuLoader(this, menu);
        connect(objectMenu_, SIGNAL(signalCreateObject(JsonValue)), this, SLOT(slotCreateNode(JsonValue)));
        return true;
    }

    bool Framework::createComponentMenuComponent()
    {
        QMenu *menu = MainWindow::instance()->ui->menuComponent;
        componentMenu_ = new MenuLoader(this, menu);
        connect(componentMenu_, SIGNAL(signalCreateObject(JsonValue)), this, SLOT(slotCreateComponent(JsonValue)));
        return true;
    }

    bool Framework::createCanvasComponent()
    {
        canvasMgr_ = new CanvasMgr(this, MainWindow::instance()->ui);
        connect(this, SIGNAL(signalRootSet(cocos2d::Node*)), canvasMgr_, SLOT(onRootSet(cocos2d::Node*)));
        connect(this, SIGNAL(signalTargetSet(cocos2d::Node*)), canvasMgr_, SLOT(onTargetSet(cocos2d::Node*)));
        return true;
    }

    bool Framework::createResourceComponent()
    {
        resource_ = new ResourceMgr(this);
        return true;
    }

    bool Framework::createResourceFolderComponent()
    {
        resourceFolder_ = new ResourceFolder(this, MainWindow::instance()->ui->resourceTree);

        connect(resource_, SIGNAL(signalAddFile(QString)), resourceFolder_, SLOT(slotAddFile(QString)));
        connect(resource_, SIGNAL(signalRemoveFile(QString)), resourceFolder_, SLOT(slotRemoveFile(QString)));
        connect(resource_, SIGNAL(signalRenameFile(QString,QString)), resourceFolder_, SLOT(slotRenameFile(QString,QString)));
        return true;
    }

    bool Framework::createResourceContentComponent()
    {
        auto table = MainWindow::instance()->ui->resourceContentTable;
        auto form = MainWindow::instance()->ui->resourceFolderWidget;
        resourceContent_ = new ResourceContent(this, table, form);

        connect(resource_, SIGNAL(signalAddFile(QString)), resourceContent_, SLOT(slotAddFile(QString)));
        connect(resource_, SIGNAL(signalRemoveFile(QString)), resourceContent_, SLOT(slotRemoveFile(QString)));
        connect(resource_, SIGNAL(signalRenameFile(QString,QString)), resourceContent_, SLOT(slotRenameFile(QString,QString)));

        connect(resourceFolder_, SIGNAL(signalShowPath(QString)), resourceContent_, SLOT(slotShowDirectory(QString)));
        return true;
    }

    bool Framework::createImporterComponent()
    {
        importer_ = new Importer(this);
        return true;
    }

    bool Framework::createLogOutComponent()
    {
        logout_ = new LogOut(this);
        return true;
    }

    bool Framework::createPluginComponent()
    {
        plugin_ = new PluginMgr(this);
        return true;
    }

    bool Framework::createStatusBarComponent()
    {
        statusBar_ = new StatusBar(MainWindow::instance()->ui->statusBar, this);
        return true;
    }

    void Framework::lockGL()
    {
        MainWindow::instance()->ui->cocos_widget->makeCurrent();
    }

    void Framework::unlockGL()
    {
        MainWindow::instance()->ui->cocos_widget->doneCurrent();
    }

    bool Framework::setResourcePath(const QString &resourcePath)
    {
        QString path = QDir::cleanPath(resourcePath);
        if(path == resourcePath_)
        {
            return true;
        }
        LOG_DEBUG("setResourcePath '%s'", path.toUtf8().data());

        resourcePath_ = path;

        std::vector<std::string> searchPath;
        searchPath.push_back(resourcePath_.toStdString());

        std::string editorResPath = ProjectManager::instance()->getEditorResourcePath().toStdString();
        LOG_DEBUG("Editor resource path: %s", editorResPath.c_str());

        searchPath.push_back(editorResPath);
        cocos2d::FileUtils::getInstance()->setSearchPaths(searchPath);

        if(resource_ != nullptr)
        {
            if(!resource_->openDirectory(resourcePath_))
            {
                return false;
            }

            if(!resourceFolder_->openDirectory(resourcePath_))
            {
                return false;
            }

            if(!resourceContent_->openDirectory(resourcePath_))
            {
                return false;
            }
        }
        return true;
    }

    void Framework::slotCreateNode(const JsonValue &config)
    {
        pushCommand(new CmdCreateNode(config, rootNode_));
    }

    void Framework::slotCreateComponent(const JsonValue &config)
    {
        //TODO support undo.
        if(nullptr == targetNode_)
        {
            return;
        }

        cocos2d::Component *component = document_->createComponent(targetNode_, config);
        if(component != nullptr)
        {
            JsonValue configCopy = config.deepClone();

            LOG_DEBUG("add component '%s'", component->getName().c_str());
            JsonValue &componentsConfig = targetConfig_["components"];
            if(componentsConfig.isNull())
            {
                componentsConfig.setDict();
            }
            componentsConfig[component->getName()] = configCopy;

            setModified(true);
            emit signalComponentCreated(component->getName());
        }
    }

    void Framework::setModified(bool modify)
    {
        if(isModified_ != modify)
        {
            isModified_ = modify;
            refreshTitle();
        }
    }

    void Framework::refreshTitle()
    {
        QString title = tr("Editor");
        title += " | ";

        if(document_ != nullptr && !document_->getFileName().isEmpty())
        {
            title += document_->getFileName();
        }
        else
        {
            title += tr("no-name");
        }

        if(isModified_)
        {
            title += tr("(*modified)");
        }

        MainWindow::instance()->setWindowTitle(title);
    }

    bool Framework::checkCurrentLayoutExist(bool echo)
    {
        if(!document_ || document_->getFileName().isEmpty())
        {
            if(echo)
            {
                QMessageBox::critical(nullptr, tr("Error"), tr("Please save current layout to file or open a new layout."));
            }
            return false;
        }
        return true;
    }

    bool Framework::checkCurrentLayoutModified()
    {
        if(isModified())
        {
            int ret = QMessageBox::question(nullptr, tr("Question"),
                tr("The current layout was modified, would you like to save it?"),
                QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort,
                QMessageBox::Save);

            if(QMessageBox::Abort == ret)
            {
                return false;
            }
            else if(QMessageBox::Save == ret)
            {
                MainWindow::instance()->doSaveEvent();
            }
        }
        return true;
    }

    void Framework::pushCommand(QUndoCommand *cmd)
    {
        if(document_ != nullptr)
        {
            document_->getUndoStack().push(cmd);
        }
    }


    GlLockHolder::GlLockHolder()
    {
        MainWindow::instance()->ui->cocos_widget->makeCurrent();
    }

    GlLockHolder::~GlLockHolder()
    {
        MainWindow::instance()->ui->cocos_widget->doneCurrent();
    }
}
