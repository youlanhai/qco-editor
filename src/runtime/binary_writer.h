#ifndef RUNTIME_BINARY_WRITER_H
#define RUNTIME_BINARY_WRITER_H

#include "helper.h"
#include <cstdlib>
#include <string>

namespace pip
{
    class PIP_DLL WriterBase
    {
    public:
        typedef uchar           value_type;
        typedef uchar *         pointer;
        typedef const uchar *   const_pointer;
        
        WriterBase();
        ~WriterBase();
        
        void reserve(size_t size);
        void ensure(size_t size);
        
        inline size_t capacity() const { return capacity_; }
        inline size_t size() const { return end_ - begin_; }
        inline void clear() { end_ = begin_; }
        void destroy();
        
        inline bool full() const { return size() == capacity_; }
        
        inline pointer begin(){ return begin_; }
        inline pointer end() { return end_; }
        
        inline pointer skip(size_t n)
        {
            ensure(n);
            pointer p = end_;
            end_ += n;
            return p;
        }
        
        inline size_t tell() const { return size(); }
        
        inline void seek(size_t index)
        {
            reserve(index);
            end_ = begin_ + index;
        }
        
        inline void write(value_type v)
        {
            ensure(sizeof(v));
            *(end_++) = v;
        }
        
        void writeBuffer(const void * pSrc, size_t bytes)
        {
            ensure(bytes);
            memcpy(end_, pSrc, bytes);
            end_ += bytes;
        }
        
        
        void writeNumber(int8 v){ write(v); }
        void writeNumber(uint8 v){ write(v); }
        
        void writeBool(bool v){ write(v ? 1: 0); }
        void writeByte(uchar ch){ write(ch); }
        
        void writeBytes(const std::string &bytes)
        {
            writeBuffer(bytes.c_str(), bytes.size());
        }

        std::string getData()
        {
            if(begin_ != nullptr)
            {
                return std::string((const char*)begin_, (const char*)end_);
            }
            return "";
        }
        
    protected:
        
        pointer         begin_;
        pointer         end_;
        size_t          capacity_;
        
    private:
        WriterBase(const WriterBase &);
        const WriterBase & operator = (const WriterBase &);
    };
    
#define DEFINE_COMMON_READER_METHOD()                   \
    void writeInt8(int8 v)      { writeNumber(v); }     \
    void writeInt16(int16 v)    { writeNumber(v); }     \
    void writeInt32(int32 v)    { writeNumber(v); }     \
    void writeInt64(int64 v)    { writeNumber(v); }     \
    void writeUint8(uint8 v)    { writeNumber(v); }     \
    void writeUint16(uint16 v)  { writeNumber(v); }     \
    void writeUint32(uint32 v)  { writeNumber(v); }     \
    void writeUint64(uint64 v)  { writeNumber(v); }     \
    void writeFloat(float v)    { writeNumber(v); }     \
    void writeDouble(double v)  { writeNumber(v); }     \
    void writeUtf8(const std::string &v){ writeString<uint16>(v); }
    
    
    class PIP_DLL WriterSameEndian : public WriterBase
    {
    public:
        WriterSameEndian()
        {}
        
        ~WriterSameEndian()
        {}
        
        using WriterBase::writeNumber;
        
        //NOTE: don't use expression 'T ret = *(T*)p', because this operation
        //will cause crash on IOS, when the p isn't a 4 or 8 byte aligned pointer.
        template<typename T>
        void writeNumber(T v)
        {
            pointer pDst = skip(sizeof(T));
            pointer pSrc = pointer(&v);
            for(size_t i = 0; i < sizeof(T); ++i)
            {
                *pDst++ = *pSrc++;
            }
        }
        
        template<typename TLength>
        void writeString(const std::string & v)
        {
            ensure(v.size() + sizeof(TLength));
            
            writeNumber((TLength)v.size());
            writeBytes(v);
        }

        DEFINE_COMMON_READER_METHOD()
    };
    
    class PIP_DLL WriterDifferentEndian : public WriterBase
    {
    public:
        WriterDifferentEndian()
        {}
        
        ~WriterDifferentEndian()
        {}
        
        using WriterBase::writeNumber;
        
        template<typename T>
        void writeNumber(T v)
        {
            pointer pDst = skip(sizeof(T));
            pointer pSrc = pointer(&v + 1);
            for(size_t i = 0; i < sizeof(T); ++i)
            {
                *pDst++ = *--pSrc;
            }
        }
        
        template<typename TLength>
        void writeString(const std::string & v)
        {
            ensure(v.size() + sizeof(TLength));
            
            writeNumber((TLength)v.size());
            writeBytes(v);
        }

        DEFINE_COMMON_READER_METHOD()
    };
    
    // the default byte order of c++ aplication is little-endian,
    // but the byte order of pip file is big-endian.
    typedef WriterDifferentEndian BinaryWriter;
    
} // end namespace pip

#endif /* defined(RUNTIME_BINARY_WRITER_H) */
