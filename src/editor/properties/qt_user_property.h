#ifndef QT_USER_PROPERTY_H
#define QT_USER_PROPERTY_H

#include "../config.h"
#include <qtpropertytype.h>

class QtPropertyFactory;
class QtPropertyEditorFactory;

namespace Editor
{

    namespace UserPropertyType
    {

        EDITOR_DLL extern const QtPropertyType::Type TYPE_COMPONENT;
        EDITOR_DLL extern const QtPropertyType::Type TYPE_SCRIPT;
        EDITOR_DLL extern const QtPropertyType::Type TYPE_FONT;


        EDITOR_DLL void registerProperties(QtPropertyFactory *factory);
        EDITOR_DLL void registerEditors(QtPropertyEditorFactory *factory);
    }

}

#endif // QT_USER_PROPERTY_H

