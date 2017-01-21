#include "helper.h"
#include <cstdlib>

NS_PIP_BEGIN

MemoryHolder::~MemoryHolder()
{
    if(nullptr != p_)
    {
        free(p_);
    }
}

NS_PIP_END
