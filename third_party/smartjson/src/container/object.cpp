//
//  object.cpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#include "object.hpp"
#include "allocator.hpp"

namespace mjson
{
    Object::Object(IAllocator *allocator)
    : allocator_(allocator)
    {
        allocator_->retain();
    }
    
    Object::~Object()
    {
        allocator_->release();
    }
    
    void Object::release()
    {
        if(--counter_ <= 0)
        {
            allocator_->freeObject(this);
        }
    }
}
