//
//  iterator.h
//  smartjson
//
//  Created by youlanhai on 16/5/7.
//  Copyright © 2016年 youlanhai. All rights reserved.
//

#ifndef iterator_h
#define iterator_h

#include <stddef.h>

namespace mjson
{
    template<typename Container, typename Value>
    class Iterator
    {
    public:
        typedef Value  value_type;
        typedef ptrdiff_t       size_type;
        
        typedef Iterator<Container, Value>     this_type;
        
        Iterator(Container *p, size_type index)
        : container_(p)
        , index_(index)
        {}
        
        size_type index()
        {
            return index_;
        }
        
        value_type& operator*()
        {
            return container_->at(index_);
        }
        
        value_type* operator->()
        {
            return &(**this);
        }
        
        this_type& operator++()
        {
            ++index_;
            return *this;
        }
        
        this_type operator++(int)
        {
            this_type old(container_, index_);
            ++index_;
            return old;
        }
        
        this_type& operator--()
        {
            --index_;
            return *this;
        }
        
        this_type operator--(int)
        {
            this_type old(container_, index_);
            --index_;
            return old;
        }
        
        bool operator == (const this_type &other) const
        {
            return container_ == other.container_ && index_ == other.index_;
        }
        
        bool operator != (const this_type &other) const
        {
            return !(*this == other);
        }
        
        bool operator < (const this_type &other) const
        {
            return index_ < other.index_;
        }
        
        bool operator <= (const this_type &other) const
        {
            return index_ <= other.index_;
        }
        
        bool operator > (const this_type &other) const
        {
            return index_ > other.index_;
        }
        
        bool operator >= (const this_type &other) const
        {
            return index_ >= other.index_;
        }
        
        this_type operator + (size_type n) const
        {
            return this_type(container_, index_ + n);
        }
        
        this_type operator - (size_type n) const
        {
            return this_type(container_, index_ - n);
        }
        
        size_type operator - (const this_type &other) const
        {
            return index_ - other.index_;
        }
        
        this_type& operator += (size_type n)
        {
            index_ += n;
            return *this;
        }
        
        this_type& operator -= (size_type n)
        {
            index_ -= n;
            return *this;
        }
        
    private:
        Container*      container_;
        size_type       index_;
    };
}

#endif /* iterator_h */
