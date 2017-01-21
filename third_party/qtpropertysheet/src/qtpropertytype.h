#ifndef QTPROPERTYTYPE_H
#define QTPROPERTYTYPE_H

#include "qtpropertyconfig.h"
#include <QString>

namespace QtPropertyType
{
	typedef QString Type;
    QTPROPERTYSHEET_DLL extern const Type NONE;
    QTPROPERTYSHEET_DLL extern const Type BOOL;
    QTPROPERTYSHEET_DLL extern const Type INT;
    QTPROPERTYSHEET_DLL extern const Type FLOAT;
    QTPROPERTYSHEET_DLL extern const Type STRING;
    QTPROPERTYSHEET_DLL extern const Type GROUP;
    QTPROPERTYSHEET_DLL extern const Type LIST;
    QTPROPERTYSHEET_DLL extern const Type DICT;
    QTPROPERTYSHEET_DLL extern const Type ENUM;
    QTPROPERTYSHEET_DLL extern const Type FLAG;
    QTPROPERTYSHEET_DLL extern const Type COLOR;
    QTPROPERTYSHEET_DLL extern const Type FILE;
    QTPROPERTYSHEET_DLL extern const Type DYNAMIC_LIST;
    QTPROPERTYSHEET_DLL extern const Type DYNAMIC_ITEM;
    QTPROPERTYSHEET_DLL extern const Type ENUM_PAIR;
    QTPROPERTYSHEET_DLL extern const Type FLOAT_LIST;
}

#endif // QTPROPERTYTYPE_H
