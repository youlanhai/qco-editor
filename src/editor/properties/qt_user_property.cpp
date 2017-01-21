#include "qt_user_property.h"
#include "qt_file_property_ex.h"
#include "qt_comp_property.h"
#include "qt_font_property.h"

#include <qtpropertyfactory.h>
#include <qtpropertyeditorfactory.h>

namespace Editor
{

    namespace UserPropertyType
    {
        const QtPropertyType::Type TYPE_COMPONENT = "component";
        const QtPropertyType::Type TYPE_SCRIPT = "script";
        const QtPropertyType::Type TYPE_FONT = "font";

        void registerProperties(QtPropertyFactory *factory)
        {
            factory->registerSimpleCreator<QtFilePropertyEx>(QtPropertyType::FILE);
            factory->registerSimpleCreator<QtGroupProperty>(UserPropertyType::TYPE_COMPONENT);
        }

        void registerEditors(QtPropertyEditorFactory *factory)
        {
            factory->registerCreator<QtFileEditorEx>(QtPropertyType::FILE);
            factory->registerCreator<QtComponentEditor>(UserPropertyType::TYPE_COMPONENT);
            factory->registerCreator<QtFontEditor>(UserPropertyType::TYPE_FONT);
        }
    }
}
