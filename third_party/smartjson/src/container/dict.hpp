//
//  dict.hpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef dict_hpp
#define dict_hpp

#include "object.hpp"
#include "iterator.h"

namespace mjson
{
    class Node;
    struct NodePair;
    
    class SMARTJSON_DLL Dict : public Object
    {
    public:
        typedef NodePair                value_type;
        typedef NodePair*               pointer;
        typedef Iterator<Dict, NodePair>          iterator;
        typedef Iterator<const Dict, const NodePair>    const_iterator;
        
        Dict(IAllocator *allocator);
        ~Dict();
        
        iterator begin();
        iterator end();
        
        const_iterator begin() const;
        const_iterator end() const;
        
        void remove(const char *key);
        void erase(iterator it);
        
        iterator find(const char *key);
        const_iterator find(const char *key) const;
        
        bool exist(const char *key) const;
        
        Node& at(const char *key);
        const Node& at(const char *key) const;
        
        value_type& at(size_t index);
        const value_type& at(size_t key) const;
        
        Node& operator[] (const char *key);
        const Node& operator[] (const char *key) const;
        
        // if the key exist, replace it's value. else insert the key-value pair to end.
        iterator insert(const char *key, const Node &value);
        
        // insert the key-value pair to end directly.
        void append(const Node &key, const Node &value);
        
        bool empty() const;
        size_t size() const;
        size_t capacity() const;
        
        void reserve(size_t size);
        void clear();
        
        bool equal(const Dict *p) const;
        
        virtual Type type() const;
        virtual Object* clone() const;
        virtual Object* deepClone() const;
        
    private:
        void ensure(size_t n);
        
        value_type*     begin_;
        value_type*     end_;
        size_t          capacity_;
    };
}

#if JSON_CODE_INLINE
#include "dict.ipp"
#endif

#endif /* dict_hpp */
