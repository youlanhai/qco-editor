#include "binary_writer.h"
#include <algorithm>

namespace pip
{

    WriterBase::WriterBase()
        : begin_(nullptr)
        , end_(nullptr)
        , capacity_(0)
    {
        
    }
    
    WriterBase::~WriterBase()
    {
        destroy();
    }
    
    void WriterBase::destroy()
    {
        if(begin_ != nullptr)
        {
            free(begin_);
            begin_ = end_ = nullptr;
            capacity_ = 0;
        }
    }
    
    void WriterBase::reserve(size_t capacity)
    {
        if(capacity > capacity_)
        {
            size_t size = end_ - begin_;
            
            if(begin_ != nullptr)
            {
                begin_ = (pointer)realloc(begin_, capacity);
            }
            else
            {
                begin_ = (pointer)malloc(capacity);
            }
            
            end_ = begin_ + size;
            capacity_ = capacity;
        }
    }
    
    void WriterBase::ensure(size_t size)
    {
        if(this->size() + size > capacity_)
        {
            size_t newCapacity = std::max((size_t)32, capacity_ << 1);
            newCapacity = std::max(newCapacity, this->size() + size);
            
            reserve(newCapacity);
        }
    }
    
} // end namespace pip
