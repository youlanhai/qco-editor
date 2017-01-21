#include "math_tool.h"

NS_PIP_BEGIN

float vector2yaw(float dx, float dy)
{
    float angle = atan2(dy, dx);
    if(angle < 0.f)
    {
        angle += PI_x2;
    }
    
    return angle;
}

Vec2 yaw2vector(float yaw)
{
    Vec2 vector;
    vector.x = cos(yaw);
    vector.y = sin(yaw);
    return vector;
}
    
NS_PIP_END
