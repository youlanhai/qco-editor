//
//  node.ipp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "container/object.hpp"
#include <cassert>

namespace mjson
{

#define JSON_UINT_TO_INTEGER(V) static_cast<Integer>(static_cast<UInteger>(V))
#define JSON_INTEGER_TO_UINT(V, T) static_cast<T>(static_cast<UInteger>(V))
    
    JSON_INLINE Node::Node()
    : type_(T_NULL)
    {
        value_.p = 0;
    }
    
    JSON_INLINE Node::~Node()
    {
        setNull();
    }
    
    JSON_INLINE Node::Node(bool value)
    : type_(T_BOOL)
    {
        value_.b = value;
    }

    JSON_INLINE Node::Node(int value)
    : type_(T_INT)
    {
        value_.i = static_cast<Integer>(value);
    }

    JSON_INLINE Node::Node(unsigned int value)
    : type_(T_INT)
    {
        value_.i = JSON_UINT_TO_INTEGER(value);
    }

    JSON_INLINE Node::Node(int64_t value)
    : type_(T_INT)
    {
        value_.i = static_cast<Integer>(value);
    }

    JSON_INLINE Node::Node(uint64_t value)
    : type_(T_INT)
    {
        value_.i = JSON_UINT_TO_INTEGER(value);
    }

    JSON_INLINE Node::Node(float value)
    : type_(T_FLOAT)
    {
        value_.f = (Float)value;
    }
    
    JSON_INLINE Node::Node(double value)
    : type_(T_FLOAT)
    {
        value_.f = (Float)value;
    }
    
    JSON_INLINE Node::Node(Object *p)
    : type_(p ? p->type() : T_NULL)
    {
        if(p != 0)
        {
            p->retain();
        }
        value_.p = p;
    }
    
    JSON_INLINE Node::Node(const char *str, size_t size, IAllocator *allocator)
    : type_(T_NULL)
    {
        setString(str, size, allocator);
    }
    
    JSON_INLINE Node::Node(const Node &other)
    : value_(other.value_)
    , type_(other.type_)
    {
        if(isPointer())
        {
            value_.p->retain();
        }
    }
    
    /////////////////////////////////////////////////////////////
    /// types
    /////////////////////////////////////////////////////////////
    
    JSON_INLINE bool Node::isNull() const
    {
        return type_ == T_NULL;
    }
    
    JSON_INLINE bool Node::isBool() const
    {
        return type_ == T_BOOL;
    }
    
    JSON_INLINE bool Node::isInt() const
    {
        return type_ == T_INT;
    }
    
    JSON_INLINE bool Node::isFloat() const
    {
        return type_ == T_FLOAT;
    }
    
    JSON_INLINE bool Node::isString() const
    {
        return type_ == T_STRING;
    }
    
    JSON_INLINE bool Node::isArray() const
    {
        return type_ == T_ARRAY;
    }
    
    JSON_INLINE bool Node::isDict() const
    {
        return type_ == T_DICT;
    }
    
    JSON_INLINE bool Node::isNumber() const
    {
        return type_ == T_INT || type_ == T_FLOAT;
    }

    JSON_INLINE bool Node::isPointer() const
    {
        return type_ > T_POINTER;
    }
 
    /////////////////////////////////////////////////////////////
    /// conver value to json
    /////////////////////////////////////////////////////////////
    JSON_INLINE void Node::setNull()
    {
        if(isPointer() && value_.p)
        {
            value_.p->release();
        }
        type_ = T_NULL;
        value_.p = 0;
    }
    
    JSON_INLINE const Node& Node::operator = (bool value)
    {
        setNull();
        type_ = T_BOOL;
        value_.b = value;
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (int value)
    {
        setNull();
        type_ = T_INT;
        value_.i = static_cast<Integer>(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (unsigned int value)
    {
        setNull();
        type_ = T_INT;
        value_.i = JSON_UINT_TO_INTEGER(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (int64_t value)
    {
        setNull();
        type_ = T_INT;
        value_.i = static_cast<Integer>(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (uint64_t value)
    {
        setNull();
        type_ = T_INT;
        value_.i = JSON_UINT_TO_INTEGER(value);
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (float value)
    {
        setNull();
        type_ = T_FLOAT;
        value_.f = (Float)value;
        return *this;
    }

    JSON_INLINE const Node& Node::operator = (double value)
    {
        setNull();
        type_ = T_FLOAT;
        value_.f = (Float)value;
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const char *value)
    {
        if(isPointer() && value_.p)
        {
            setString(value, 0, value_.p->getAllocator());
        }
        else
        {
            setString(value);
        }
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const Object *value)
    {
        if(!isPointer() || value_.p != value)
        {
            setNull();
            if(value != 0)
            {
                type_ = value->type();
                value_.p = const_cast<Object*>(value);
                value_.p->retain();
            }
        }
        return *this;
    }
    
    JSON_INLINE const Node& Node::operator = (const Node &value)
    {
        if(value.isPointer())
        {
            return *this = value.value_.p;
        }
        else
        {
            setNull();
            
            type_ = value.type_;
            value_ = value.value_;
            return *this;
        }
    }
    
    /////////////////////////////////////////////////////////////
    /// convert json to value safely
    /////////////////////////////////////////////////////////////
    
    JSON_INLINE bool Node::asBool() const
    {
        return isBool() ? value_.b : false;
    }
    
    JSON_INLINE Integer Node::asInteger() const
    {
        if(isInt()) return value_.i;
        if(isFloat()) return static_cast<Integer>(value_.f);
        return 0;
    }

    JSON_INLINE int Node::asInt() const
    {
        return static_cast<int>(asInteger());
    }

    JSON_INLINE unsigned int Node::asUint() const
    {
        return JSON_INTEGER_TO_UINT(asInteger(), unsigned int);
    }

    JSON_INLINE int64_t Node::asInt64() const
    {
        return static_cast<int64_t>(asInteger());
    }

    JSON_INLINE uint64_t Node::asUint64() const
    {
        return JSON_INTEGER_TO_UINT(asInteger(), uint64_t);
    }

    JSON_INLINE Float Node::asFloat() const
    {
        if(isFloat()) return value_.f;
        if(isInt()) return (Float)value_.i;
        return (Float)0;
    }
    
    JSON_INLINE String* Node::asString() const
    {
        return isString() ? value_.ps : 0;
    }

    JSON_INLINE Array* Node::asArray() const
    {
        return isArray() ? value_.pa : 0;
    }

    JSON_INLINE Dict* Node::asDict() const
    {
        return isDict() ? value_.pd : 0;
    }

    /////////////////////////////////////////////////////////////
    /// convert json to value unsafe
    /////////////////////////////////////////////////////////////
    JSON_INLINE bool Node::rawBool() const
    {
        return value_.b;
    }

    JSON_INLINE int Node::rawInt() const
    {
        return (int)value_.i;
    }

    JSON_INLINE int64_t Node::rawInt64() const
    {
        return (int64_t)value_.i;
    }

    JSON_INLINE Integer Node::rawInteger() const
    {
        return value_.i;
    }

    JSON_INLINE Float Node::rawFloat() const
    {
        return value_.f;
    }

    JSON_INLINE String* Node::rawString() const
    {
        return value_.ps;
    }

    JSON_INLINE Array* Node::rawArray() const
    {
        return value_.pa;
    }

    JSON_INLINE Dict* Node::rawDict() const
    {
        return value_.pd;
    }

    /////////////////////////////////////////////////////////////
    /// convert json to value
    /////////////////////////////////////////////////////////////
    JSON_INLINE bool Node::operator != (const Node &value) const
    {
        return !(*this == value);
    }
    
    JSON_INLINE const Node& Node::operator[] (SizeType index) const
    {
        return const_cast<Node*>(this)->operator[](index);
    }
    
    JSON_INLINE const Node& Node::operator[] (const char *key) const
    {
        return find(key);
    }

    JSON_INLINE const Node& Node::operator[] (const Node &key) const
    {
        return find(key.asCString());
    }

    JSON_INLINE Node Node::clone() const
    {
        Node ret;
        
        ret.type_ = type_;
        ret.value_ = value_;
        if(isPointer())
        {
            ret.value_.p = value_.p->clone();
            ret.value_.p->retain();
        }
        return ret;
    }
    
    JSON_INLINE Node Node::deepClone() const
    {
        Node ret;
        
        ret.type_ = type_;
        ret.value_ = value_;
        if(isPointer())
        {
            ret.value_.p = value_.p->deepClone();
            ret.value_.p->retain();
        }
        return  ret;
    }
    
#if JSON_SUPPORT_STL_STRING
    JSON_INLINE Node::Node(const std::string &value, IAllocator *allocator)
    : type_(T_NULL)
    {
        setString(value.c_str(), value.size(), allocator);
    }
    
    JSON_INLINE const Node& Node::operator = (const std::string &value)
    {
        setString(value.c_str(), value.size());
        return *this;
    }
    
    JSON_INLINE Node& Node::operator[] (const std::string &key)
    {
        return (*this)[key.c_str()];
    }

    JSON_INLINE const Node& Node::operator[] (const std::string &key) const
    {
        return find(key.c_str());
    }
    
    JSON_INLINE void Node::setStdString(const std::string &value, IAllocator *allocator)
    {
        return setString(value.c_str(), value.size(), allocator);
    }
   
#endif
    
}
