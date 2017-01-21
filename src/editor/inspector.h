#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "config.h"
#include <QObject>
#include <QMap>
#include <QString>
#include <unordered_map>

#include <base/CCRefPtr.h>
#include <runtime/json_helper.h>

//predefination
NS_CC_BEGIN
class Node;
class Component;
NS_CC_END

class QDockWidget;
class QMenu;

class QtProperty;
class QtPropertyEditorFactory;
class QtPropertyBrowser;

class PropertyParam;

namespace Editor
{
    class PropertyGroup;

    class EDITOR_DLL Inspector : public QObject
    {
        Q_OBJECT
    public:
        explicit Inspector(QObject *parent, QDockWidget *view);
        ~Inspector();

        QtProperty* findQtProperty(const std::string &name);
        QtProperty* findComponentProperty(const std::string &name);

        QtPropertyEditorFactory* getPropertyEditorFactory(){ return editorFactory_; }

    public slots:
        void onTargetSet(cocos2d::Node *target);
        void onPropertyChange(QtProperty *property);
        void onPropertyChange(PropertyParam &param);

        void onComponentAdd(const std::string &name);
        void onComponentPropertyChange(QtProperty *property);
        void onComponentPropertyChange(PropertyParam &param);
        void onComponentPropertyRemove(QtProperty *property, QtProperty *parent);
        void onComponentRename(cocos2d::Component *p, const std::string &oldName);

        void onPropertyTreeDestroyed(QObject*);

        void onPropertyPopupMenu(QtProperty *property);
        void onActionRemoveComponent();

    private:
        void cleanup();
        void createPropertyView();
        void createComponentView();
        void bindNameAndProperty();

        void applyDataToView();
        void applyConfigToView(const JsonValue &config, bool dispatchEvent);
        void applyComponentConfigToView(cocos2d::Component *p, QtProperty *property, const JsonValue &config, PropertyGroup *group);

        void applyPropertyChangeToView(const std::string &name, const JsonValue &value, bool dispatchEvent);
        void applyComponentPropertyToView(QtProperty *property, PropertyParam &param, PropertyGroup *group);

        void recordExpandStatus();
        void applyExpandStatus();

        void connectSignal();
        void disconnectSignal();

        bool isPropertyChangedByHand() const { return propertyChangedByHand_ != 0; }

        void createPropertyBrowser(QWidget *view);
        void createTreeBrowser(QWidget *view);
        void createButtonBrowser(QWidget *view);

        QtProperty* addComponent(const std::string &name, const JsonHandle &config);

        cocos2d::RefPtr<cocos2d::Node> targetNode_;
        JsonHandle                  targetConfig_;
        PropertyGroup*              targetPropertyGroup_;

        QtPropertyEditorFactory*    editorFactory_;
        QtPropertyBrowser*          propertyTree_;

        typedef std::vector<QtProperty*> PropertyArray;
        PropertyArray               propertyArray_;
        PropertyArray               componentArray_;

        typedef std::unordered_map<std::string, QtProperty*> PropertyMap;
        PropertyMap                 name2property_;

        typedef QMap<QString, bool> ExpandStatus;
        ExpandStatus                expandStatus_;

        int                         propertyChangedByHand_;

        QMenu*                      componentMenu_;
        QtProperty*                 menuProperty_;
    };

} // end namespace Editor

#endif // INSPECTOR_H
