#ifndef PROPERTY_CREATOR_H
#define PROPERTY_CREATOR_H

#include "../config.h"
#include <string>
#include <runtime/singleton.h>
#include <qtpropertyfactory.h>

namespace Editor
{
    class PropertyDefinition;
    class PropertyCreatorMgr;

    class EDITOR_DLL PropertyCreatorMgr : public QtPropertyFactory, public Singleton<PropertyCreatorMgr>
    {
    public:
        PropertyCreatorMgr();
        ~PropertyCreatorMgr();

        /** return new QtProperty instance, caller has responsibility for delete it.*/
        QtProperty* createPropertyByName(const std::string &name);

        /** return new QtProperty instance, caller has responsibility for delete it.*/
        QtProperty* createPropertyByDef(const PropertyDefinition *tp);

        bool registerCreatorFile(const std::string &filename);

        QtPropertyFactory* getPropertyMgr(){ return this; }
    };

}
#endif // PROPERTY_CREATOR_H
