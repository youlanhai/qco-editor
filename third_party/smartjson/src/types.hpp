//
//  types.hpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef types_h
#define types_h

#include "config.hpp"

#include <float.h>
#include <stdint.h>
#include <stdlib.h>

namespace mjson
{
    typedef unsigned int SizeType;
    
#if JSON_USE_LARGE_NUMBER
    typedef int64_t     Integer;
    typedef uint64_t    UInteger;
    typedef double      Float;
    const Float DefaultEpsilon = DBL_EPSILON;
#else
    typedef int32_t     Integer;
    typedef int32_t     UInteger;
    typedef float       Float;
    const Float DefaultEpsilon = FLT_EPSILON;
#endif

    enum Type
    {
        T_NULL,
        T_BOOL,
        T_INT,
        T_FLOAT,
        
        T_POINTER,
        T_STRING,
        T_ARRAY,
        T_DICT,
    };

} // end namespace mjson

#endif /* types_h */
