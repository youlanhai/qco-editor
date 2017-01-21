#ifndef __Editor__EditorValueTypeMgr__
#define __Editor__EditorValueTypeMgr__

#include "../config.h"
#include <platform/CCCommon.h>
#include <smartjson/src/node.hpp>
#include <runtime/singleton.h>

#include <string>
#include <unordered_map>
#include <vector>

class JsonHandle;
class QtProperty;
class PropertyParam;

namespace Editor
{
    typedef QtProperty IProperty;
    class PropertyDefinition;
    class PropertyListener;
    
    class EDITOR_DLL PropertyGroup
    {
    public:
        PropertyGroup();
        ~PropertyGroup();
        
        bool load(const JsonHandle &config);

        const std::string& getName() const{ return name_; }
        PropertyGroup* getParent(){ return parent_; }

        IProperty* getProperty(){ return property_; }
        IProperty* findChildProperty(const std::string &name);
        IProperty* findChildPropertyRecursively(const std::string &name);

        IProperty* cloneProperty() const;
        
        const PropertyDefinition* getDefinition() const { return definition_; }

        void sortProperty(mjson::Node &config) const;

        void addListener(const std::string &key, PropertyListener *listener);
        void notifyPropertyChange(PropertyParam &param);

        mjson::Node findDefaultValue(const std::string &key);
        mjson::Node getDefaultConfig() const { return defaultConfig_; }

    private:
        bool loadBridge(JsonHandle config);
        bool loadSaveOrder(JsonHandle config);

        std::string         name_;
        PropertyGroup*      parent_;
        PropertyDefinition* definition_;
        IProperty*          property_;

        typedef std::vector<std::vector<std::string>> SaveOrders;
        SaveOrders          saveOrders_;

        typedef std::vector<PropertyListener*>  ListenerVector;
        typedef std::unordered_map<std::string, ListenerVector> ListenerMap;
        ListenerMap         listeners_;

        mjson::Node         defaultConfig_;
    };
    
    class EDITOR_DLL PropertyGroupMgr : public Singleton<PropertyGroupMgr>
    {
    public:
        PropertyGroupMgr();
        ~PropertyGroupMgr();
        
        bool registerGroupFile(const std::string &filename);
        PropertyGroup* findGroup(const std::string &name);
        
        const std::string & cppNameToUIName(const std::string &cppName);
        void registerUIName(const std::string &className, const std::string &uiName);
        
    private:
        bool registerGroup(const JsonHandle & value);
        
        typedef std::unordered_map<std::string, PropertyGroup*> PropertyMap;
        PropertyMap groups_;
        
        typedef std::unordered_map<std::string, std::string> StringMap;
        StringMap   class2uiName_;
    };
}

#endif /* defined(__Editor__EditorValueTypeMgr__) */
