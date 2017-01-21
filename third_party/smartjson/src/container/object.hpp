//
//  object.hpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef object_hpp
#define object_hpp

#include "reference.hpp"
#include "../types.hpp"

namespace mjson
{
    class IAllocator;
    
    class SMARTJSON_DLL Object : public Reference
    {
        JSON_DISABLE_COPY_ASSIGN(Object);
    public:
        Object(IAllocator *allocator);
        ~Object();
        
        virtual void    release();
        virtual Type    type() const = 0;
        virtual Object* clone() const = 0;
        virtual Object* deepClone() const = 0;
        
        IAllocator* getAllocator(){ return allocator_; }
        
    protected:
        IAllocator*     allocator_;
    };
}

#endif /* object_hpp */
