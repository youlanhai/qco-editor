//
//  string.hpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef string_hpp
#define string_hpp

#include "object.hpp"

#if JSON_SUPPORT_STL_STRING
#include <string>
#endif

namespace mjson
{
    class SMARTJSON_DLL String : public Object
    {
    public:
        String(const char *str, size_t size, IAllocator *allocator);
        String(char *str, size_t size, bool placeHolder, IAllocator *allocator);
        ~String();
        
        int compare(const char *str) const;
        int compare(const char *str, size_t length) const;
        int compare(const String *p) const;
#if JSON_SUPPORT_STL_STRING
        int compare(const std::string &str) const;
#endif
        
        size_t size() const;
        const char* data() const;
        
        virtual Type type() const;
        
        virtual Object* clone() const;
        virtual Object* deepClone() const;
        
    private:
        char*           str_;
        size_t          size_;
    };
}

#if JSON_CODE_INLINE
#include "string.ipp"
#endif

#endif /* string_hpp */
