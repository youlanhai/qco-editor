#ifndef __Editor__EditorPropertyValue__
#define __Editor__EditorPropertyValue__


#include "../config.h"
#include <string>
#include <vector>
#include <map>

#include <QVariant>

class JsonHandle;
namespace Editor
{
    class EDITOR_DLL PropertyDefinition
    {
    public:
        PropertyDefinition();
        ~PropertyDefinition();
        
        bool load(const JsonHandle &config);
        bool loadChildren(const JsonHandle & config);

        PropertyDefinition* clone() const;
        void merge(const PropertyDefinition *src);
        
        std::string         type_;
        std::string         key_;
        std::string         name_;
        std::string         desc_;

        typedef QMap<QString, QVariant> Attributes;
        Attributes          attributes_;
        
        typedef std::vector<PropertyDefinition*> Children;
        Children            items_;

    private:
        bool loadTypedef(const JsonHandle & config);
        bool loadValue(const JsonHandle & config);

        // disable copy construct and assign, user clone instead.
        PropertyDefinition(const PropertyDefinition&);
        const PropertyDefinition& operator = (const PropertyDefinition&);
    };

} // end namespace Editor

#endif /* defined(__Editor__EditorPropertyValue__) */
