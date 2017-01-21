#ifndef RUNTIME_HELPER_H
#define RUNTIME_HELPER_H

#include "pip_config.h"
#include <cstdint>

NS_PIP_BEGIN

typedef signed char     schar;
typedef unsigned char   uchar;

typedef int8_t      int8;
typedef uint8_t     uint8;
typedef int16_t     int16;
typedef uint16_t    uint16;
typedef int32_t     int32;
typedef uint32_t    uint32;
typedef int64_t     int64;
typedef uint64_t    uint64;

/** 托管由malloc分配的内存。对象析构的时候，自动free掉内存。*/
class PIP_DLL MemoryHolder
{
public:
    explicit MemoryHolder(void *p)
    : p_(p)
    {}
    
    ~MemoryHolder();
    
private:
    void *p_;
};

NS_PIP_END

#define CC_DEFAULT_CREATE(CLASS) \
static CLASS* create()\
{\
    CLASS *p = new (std::nothrow)CLASS();\
    if(p && p->init())\
    {\
        p->autorelease(); \
    }\
    else\
    {\
        delete p; \
        p = nullptr; \
    }\
    return p;\
}

#define UNUSED_PARAM(NAME, VALUE) NAME = VALUE



#endif /* defined(RUNTIME_HELPER_H) */
