#include "binary_reader.h"

namespace pip
{
    ReaderBase::ReaderBase() :
        begin_(nullptr),
        end_(nullptr),
        ptr_(nullptr),
        managed_(false)
    {
        
    }
    
    ReaderBase::~ReaderBase()
    {
        if(managed_)
        {
            delete [] const_cast<pointer>(begin_);
        }
    }
    
    void ReaderBase::assign(const_pointer begin, size_t size, bool managed)
    {
        ptr_ = begin_ = begin;
        end_ = begin_ + size;
        
        managed_ = managed;
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ReaderSameEndian::ReaderSameEndian()
    {
        
    }

    ReaderSameEndian::~ReaderSameEndian()
    {
        
    }
    
    template<> uint8 ReaderSameEndian::readNumber()
    {
        return (uint8)this->read();
    }
    
    template<> int8 ReaderSameEndian::readNumber()
    {
        return (int8)this->read();
    }
    
    ////////////////////////////////////////////////////////////////////////////
    ReaderDifferentEndian::ReaderDifferentEndian()
    {
        
    }
    
    ReaderDifferentEndian::~ReaderDifferentEndian()
    {
        
    }
    
    template<> uint8 ReaderDifferentEndian::readNumber()
    {
        return (uint8)this->read();
    }
    
    template<> int8 ReaderDifferentEndian::readNumber()
    {
        return (int8)this->read();
    }

} // end namespace pip
