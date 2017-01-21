//
//  node.cpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "node.hpp"
#include "container/string.hpp"
#include "container/array.hpp"
#include "container/dict.hpp"
#include "container/allocator.hpp"
#include <cmath>

#if !JSON_CODE_INLINE
#include "node.ipp"
#endif

namespace mjson
{
    
    const char* Node::asCString() const
    {
        return isString() ? rawString()->data() : "";
    }

    const char* Node::rawCString() const
    {
        JSON_ASSERT(isString());
        return rawString()->data();
    }
    
    String& Node::refString()
    {
        JSON_ASSERT(isString());
        return *(value_.ps);
    }

    Array& Node::refArray()
    {
        JSON_ASSERT(isArray());
        return *(value_.pa);
    }

    Dict& Node::refDict()
    {
        JSON_ASSERT(isDict());
        return *(value_.pd);
    }

    const String& Node::refString() const
    {
        JSON_ASSERT(isString());
        return *(value_.ps);
    }

    const Array& Node::refArray() const
    {
        JSON_ASSERT(isArray());
        return *(value_.pa);
    }

    const Dict& Node::refDict() const
    {
        JSON_ASSERT(isDict());
        return *(value_.pd);
    }

    void Node::setString(const char *str, size_t size, IAllocator *allocator)
    {
        if(0 == allocator)
        {
            allocator = RawAllocator::defaultAllocator();
        }
        allocator->retain();
        
        if(0 == size)
        {
            size = strlen(str);
        }
        
        setNull();
        type_ = T_STRING;
        
        value_.p = allocator->createString(str, size);
        value_.p->retain();
        
        allocator->release();
    }
    
    Array* Node::setArray(IAllocator *allocator)
    {
        setNull();
        type_ = T_ARRAY;
        
        if(0 == allocator)
        {
            allocator = RawAllocator::defaultAllocator();
        }
        allocator->retain();
        
        value_.p = allocator->createArray();
        value_.p->retain();
        
        allocator->release();
        return (Array*)value_.p;
    }
    
    Dict* Node::setDict(IAllocator *allocator)
    {
        setNull();
        type_ = T_DICT;
        
        if(0 == allocator)
        {
            allocator = RawAllocator::defaultAllocator();
        }
        allocator->retain();
        
        value_.p = allocator->createDict();
        value_.p->retain();
        
        allocator->release();
        return (Dict*)value_.p;
    }
    
    bool Node::operator == (const Node &other) const
    {
        if(this->isNumber() && other.isNumber())
        {
            if(this->isFloat() && other.isFloat())
            {
                return fabs(value_.f - other.value_.f) < DefaultEpsilon;
            }
            else if(this->isFloat())
            {
                return fabs(value_.f - (Float)other.value_.i) < DefaultEpsilon;
            }
            else if(other.isFloat())
            {
                return fabs((Float)value_.i - other.value_.f) < DefaultEpsilon;
            }
            else
            {
                return value_.i == other.value_.i;
            }
        }
        
        if(type_ != other.type_)
        {
            return false;
        }
        
        if(type_ == T_NULL)
        {
            return true;
        }
        else if(type_ == T_BOOL)
        {
            return value_.b == other.value_.b;
        }
        else if(type_ == T_STRING)
        {
            return rawString()->compare(other.rawString()) == 0;
        }
        else if(type_ == T_ARRAY)
        {
            return rawArray()->equal(other.rawArray());
        }
        else if(type_ == T_DICT)
        {
            return rawDict()->equal(other.rawDict());
        }
        else
        {
            assert(false && "shouldn't reach here.");
            return false;
        }
    }
    
    size_t Node::size() const
    {
        if(isArray())
        {
            return rawArray()->size();
        }
        else if(isDict())
        {
            return rawDict()->size();
        }
        else if(isString())
        {
            return rawString()->size();
        }
        return 0;
    }
    
    
    Node& Node::operator[] (SizeType index)
    {
        if(isArray() && index < rawArray()->size())
        {
            return (*rawArray())[index];
        }
        
        static Node null;
        null.setNull();
        return null;
    }
    
    Node& Node::operator[] (const char *key)
    {
        if(isDict())
        {
            return (*rawDict())[key];
        }
        
        static Node null;
        null.setNull();
        return null;
    }

    Node& Node::operator[] (const Node &key)
    {
        if(isDict())
        {
            return (*rawDict())[key.asCString()];
        }

        static Node null;
        null.setNull();
        return null;
    }

    const Node& Node::find(const char *key) const
    {
        if(isDict())
        {
            Dict::iterator it = rawDict()->find(key);
            if(it != rawDict()->end())
            {
                return it->value;
            }
        }

        static Node null;
        null.setNull();
        return null;
    }

#if JSON_SUPPORT_STL_STRING
    
    void Node::asStdString(std::string &out) const
    {
        if(isString())
        {
            out.assign(rawString()->data(), rawString()->size());
        }
    }
    
    std::string Node::asStdString() const
    {
        if(isString())
        {
            return std::string(rawString()->data(), rawString()->size());
        }
        return "";
    }
    
#endif
}
