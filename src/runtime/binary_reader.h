#ifndef RUNTIME_BINARY_READER_H
#define RUNTIME_BINARY_READER_H

#include "helper.h"

#include <string>
#include <vector>
#include <algorithm>

namespace pip
{
    class PIP_DLL ReaderBase
    {
    public:
        typedef uchar           value_type;
        typedef uchar *         pointer;
        typedef const uchar *   const_pointer;
        
        ReaderBase();
        ~ReaderBase();
        
        void assign(const_pointer begin, size_t size, bool managed = false);
        
        inline size_t capacity() const { return end_ - begin_; }
        inline size_t size() const { return std::max(0, int(end_ - ptr_)); }
        
        inline bool empty() const { return ptr_ >= end_; }
        inline bool valid() const { return ptr_ != nullptr && ptr_ <= end_; }
        inline bool check(size_t n) const { return ptr_ + n <= end_; }
        
        inline const_pointer begin() const { return begin_; }
        inline const_pointer end() const { return end_; }
        inline const_pointer ptr() const { return ptr_; }
        
        inline const_pointer skip(size_t n)
        {
            const_pointer p = ptr_;
            ptr_ += n;
            return p;
        }
        
        inline size_t tell() const
        {
            return ptr_ - begin_;
        }
        
        inline void seek(size_t index)
        {
            ptr_ = begin_ + std::max(index, capacity());
        }
        
        inline value_type read()
        {
            if(ptr_ < end_)
            {
                return *ptr_++;
            }
            else
            {
                ++ptr_; // increment the ptr_ too, so we will know how much bytes needed.
                return 0;
            }
        }
        
        const void* readBuffer(void* pDst, int bytes)
        {
            if(bytes < 0)
            {
                bytes = (int)size();
            }

            const_pointer pSrc = skip((size_t)bytes);
            if(valid())
            {
                memcpy(pDst, pSrc, bytes);
                return pDst;
            }
            
            return nullptr;
        }
        
        void readBuffer(std::string & data, int bytes)
        {
            if(bytes < 0)
            {
                bytes = (int)size();
            }
            
            const char * pSrc = (const char *)skip((size_t)bytes);
            if(valid())
            {
                data.assign(pSrc, bytes);
            }
        }

        bool readBool(){ return read() != 0; }
        value_type readByte(){ return read(); }

        std::string readBytes(int n)
        {
            std::string ret;
            readBuffer(ret, n);
            return ret;
        }
        
    protected:
        const_pointer         begin_;
        const_pointer         end_;
        const_pointer         ptr_;
        bool                  managed_;
    };
    
#define DEFINE_READER_COMMON_METHOD() \
    int8    readInt8()      { return (int8)read(); }            \
    uint8   readUint8()     { return (uint8)read(); }           \
    int16   readInt16()     { return readNumber<int16>(); }     \
    uint16  readUint16()    { return readNumber<uint16>(); }    \
    int32   readInt32()     { return readNumber<int32>(); }     \
    uint32  readUint32()    { return readNumber<uint32>(); }    \
    int64   readInt64()     { return readNumber<int64>(); }     \
    uint64  readUint64()    { return readNumber<uint64>(); }    \
    float   readFloat()     { return readNumber<float>(); }     \
    double  readDouble()    { return readNumber<double>(); }    \
    std::string readUtf8()  { return readString<int16>(); }
    

    class PIP_DLL ReaderSameEndian : public ReaderBase
    {
    public:
        ReaderSameEndian();
        ~ReaderSameEndian();
        
        //NOTE: don't use expression 'T ret = *(T*)p', because this operation
        //will cause crash on IOS, when the p isn't a 4 or 8 byte aligned pointer.
        template<typename T>
        T readNumber()
        {
            const_pointer p = this->skip(sizeof(T));
            if(this->valid())
            {
                T ret;
                pointer pDst = pointer(&ret);
                for(size_t i = 0; i < sizeof(T); ++i)
                {
                    *pDst++ = *p++;
                }
                return ret;
            }
            return T();
        }
        
        template<typename TLength>
        std::string readString()
        {
            TLength length = this->readNumber<TLength>();
            const_pointer p = this->skip(length);
            
            if(this->valid())
            {
                return std::string((const char *)p, length);
            }
            else
            {
                return std::string();
            }
        }
        
        DEFINE_READER_COMMON_METHOD()
     };
    
    // the readNumber<uint8> is incorrect in release mode,
    // maybe something wrong after optimize by compiler in release mode.
    // so, explicit specialize it.
    template<> uint8 PIP_DLL ReaderSameEndian::readNumber();
    template<> int8 PIP_DLL ReaderSameEndian::readNumber();
    
    class PIP_DLL ReaderDifferentEndian : public ReaderBase
    {
    public:
        
        ReaderDifferentEndian();
        ~ReaderDifferentEndian();
        
        template<typename T>
        T readNumber()
        {
            const_pointer p = this->skip(sizeof(T));
            if(this->valid())
            {
                T ret;
                pointer pDst = pointer(&ret + 1);
                for(size_t i = 0; i < sizeof(T); ++i)
                {
                    *--pDst = *p++;
                }
                return ret;
            }
            
            return T();
        }
        
        template<typename TLength>
        std::string readString()
        {
            TLength length = this->readNumber<TLength>();
            const_pointer p = this->skip(length);
            
            if(this->valid())
            {
                return std::string((const char *)p, length);
            }
            else
            {
                return std::string();
            }
        }
        
        DEFINE_READER_COMMON_METHOD()
    };
    
    template<> uint8 PIP_DLL ReaderDifferentEndian::readNumber();
    template<> int8 PIP_DLL ReaderDifferentEndian::readNumber();
    
    // the default byte order of c++ aplication is little-endian,
    // but the byte order of pip file is big-endian.
    typedef ReaderDifferentEndian BinaryReader;

    template<typename T, typename U>
    bool readNumbers(BinaryReader & reader, std::vector<U> & datas)
    {
        if(!reader.check(datas.size() * sizeof(T)))
        {
           return false;
        }
        
        for(size_t i = 0; i < datas.size(); ++i)
        {
            datas[i] = (U)reader.readNumber<T>();
        }
        return true;
    }
    
} // end namespace pip

#endif /* defined(RUNTIME_BINARY_READER_H) */
