#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

namespace Editor
{
    enum class SceneType
    {
        TP_2D,
        TP_3D,
        TP_TILE,
    };

    enum class GizmoMode
    {
        None,
        Position,
        Rotation,
        Scale,
    };
}

#endif // COMMON_TYPES_H

