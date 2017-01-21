//
//  dict.ipp
//  smartjson
//
//  Created by youlanhai on 15/12/20.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "../node.hpp"

namespace mjson
{
    JSON_INLINE Dict::Dict(IAllocator *allocator)
    : Object(allocator)
    , begin_(0)
    , end_(0)
    , capacity_(0)
    {
        
    }
    
    JSON_INLINE Dict::iterator Dict::begin()
    {
        return iterator(this, 0);
    }
    
    JSON_INLINE Dict::iterator Dict::end()
    {
        return iterator(this, end_ - begin_);
    }
    
    JSON_INLINE Dict::const_iterator Dict::begin() const
    {
        return const_iterator(this, 0);
    }
    
    JSON_INLINE Dict::const_iterator Dict::end() const
    {
        return const_iterator(this, end_ - begin_);
    }
    
    JSON_INLINE Dict::const_iterator Dict::find(const char *key) const
    {
        iterator it = const_cast<Dict*>(this)->find(key);
        return const_iterator(this, it.index());
    }
    
    JSON_INLINE bool Dict::exist(const char *key) const
    {
        return find(key) != end();
    }
    
    JSON_INLINE const Node& Dict::at(const char *key) const
    {
        return const_cast<Dict*>(this)->at(key);
    }
    
    JSON_INLINE Node& Dict::operator[] (const char *key)
    {
        return at(key);
    }
    
    JSON_INLINE const Node& Dict::operator[] (const char *key) const
    {
        return at(key);
    }
    
    JSON_INLINE void Dict::remove(const char *key)
    {
        erase(find(key));
    }
    
    JSON_INLINE bool Dict::empty() const
    {
        return this->size() == 0;
    }
    
    JSON_INLINE size_t Dict::capacity() const
    {
        return capacity_;
    }
    
    JSON_INLINE Type Dict::type() const
    {
        return T_DICT;
    }
    
    JSON_INLINE size_t Dict::size() const
    {
        return end_ - begin_;
    }
    
    JSON_INLINE Dict::value_type& Dict::at(size_t index)
    {
        return *(begin_ + index);
    }
    
    JSON_INLINE const Dict::value_type& Dict::at(size_t index) const
    {
        return *(begin_ + index);
    }
}
