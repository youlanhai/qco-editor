#include "property_param.h"

PropertyParam::PropertyParam(ObjectPointer _object,
              const std::string & _name,
              const JsonHandle &_value,
              const JsonHandle &_properties)
    : object(_object)
    , name(_name)
    , value(_value)
    , properties(_properties)
{}
