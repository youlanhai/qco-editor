#include "editor_multi_selector.h"

#include "../framework.h"
#include "../tools/math_tool.h"

#include <runtime/json_helper.h>

#include <algorithm>

namespace Editor
{
    cocos2d::Vec3 nodeGetWorldPosition(const cocos2d::Node *node)
    {
        cocos2d::Vec3 position = node->getPosition3D();
        if(node->getParent())
        {
            node->getParent()->getNodeToWorldTransform().transformPoint(&position);
        }
        return position;
    }

    void nodeSetWorldPosition(cocos2d::Node *node, const cocos2d::Vec3 &position)
    {
        cocos2d::Vec3 localPosition = position;
        if(node->getParent() != nullptr)
        {
            node->getParent()->getWorldToNodeTransform().transformPoint(&localPosition);
        }
        node->setPosition3D(localPosition);
    }

    cocos2d::Quaternion nodeGetWorldRotation(const cocos2d::Node *node)
    {
        cocos2d::Quaternion quat;
        quat.setIdentity();

        while(node)
        {
            quat = node->getRotationQuat() * quat;
            node = node->getParent();
        }
        return quat;
    }

    void nodeSetWorldRotation(cocos2d::Node *node, const cocos2d::Quaternion &quat)
    {
        cocos2d::Quaternion toLocal = nodeGetWorldRotation(node->getParent());
        toLocal.inverse();

        node->setRotationQuat(toLocal * quat);
    }

    cocos2d::Rect nodeGetWorldRect(const cocos2d::Node *node)
    {
        cocos2d::Rect rect;
        rect.origin = cocos2d::Vec2::ZERO;
        rect.size = node->getContentSize();

        cocos2d::AffineTransform transform = node->getNodeToWorldAffineTransform();
        return cocos2d::RectApplyAffineTransform(rect, transform);
    }

    void createMatrixFromRotation(cocos2d::Mat4 &mat, cocos2d::Vec3 &rotation)
    {
        cocos2d::Mat4 temp;
        cocos2d::Mat4::createRotationX(CC_DEGREES_TO_RADIANS(rotation.x), &mat);

        cocos2d::Mat4::createRotationY(CC_DEGREES_TO_RADIANS(rotation.y), &temp);
        mat.multiply(temp);

        cocos2d::Mat4::createRotationZ(CC_DEGREES_TO_RADIANS(-rotation.z), &temp);
        mat.multiply(temp);
    }

    cocos2d::Vec3 operator*(const cocos2d::Vec3 &a, const cocos2d::Vec3 &b)
    {
        return cocos2d::Vec3(a.x * b.x, a.y * b.y, a.z * b.z);
    }

    cocos2d::Vec3 operator/(const cocos2d::Vec3 &a, const cocos2d::Vec3 &b)
    {
        return cocos2d::Vec3(b.x == 0.0f ? 0.0f : a.x / b.x,
                             b.y == 0.0f ? 0.0f : a.y / b.y,
                             b.z == 0.0f ? 0.0f : a.z / b.z);
    }

    // convert Euler angle to quaternion
    cocos2d::Quaternion euler2quat(const cocos2d::Vec3 &euler)
    {
        cocos2d::Quaternion quat;
        float halfRadx = CC_DEGREES_TO_RADIANS(euler.x / 2.f);
        float halfRady = CC_DEGREES_TO_RADIANS(euler.y / 2.f);
        float halfRadz = -CC_DEGREES_TO_RADIANS(euler.z / 2.f);

        float coshalfRadx = cosf(halfRadx);
        float sinhalfRadx = sinf(halfRadx);
        float coshalfRady = cosf(halfRady);
        float sinhalfRady = sinf(halfRady);
        float coshalfRadz = cosf(halfRadz);
        float sinhalfRadz = sinf(halfRadz);

        quat.x = sinhalfRadx * coshalfRady * coshalfRadz - coshalfRadx * sinhalfRady * sinhalfRadz;
        quat.y = coshalfRadx * sinhalfRady * coshalfRadz + sinhalfRadx * coshalfRady * sinhalfRadz;
        quat.z = coshalfRadx * coshalfRady * sinhalfRadz - sinhalfRadx * sinhalfRady * coshalfRadz;
        quat.w = coshalfRadx * coshalfRady * coshalfRadz + sinhalfRadx * sinhalfRady * sinhalfRadz;
        return quat;
    }

    //convert quaternion to Euler angle
    cocos2d::Vec3 quat2euler(const cocos2d::Quaternion &quat)
    {
        cocos2d::Vec3 euler;
        float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
        euler.x = atan2f(2.f * (w * x + y * z), 1.f - 2.f * (x * x + y * y));
        float sy = 2.f * (w * y - z * x);
        sy = cocos2d::clampf(sy, -1, 1);
        euler.y = asinf(sy);
        euler.z = atan2f(2.f * (w * z + x * y), 1.f - 2.f * (y * y + z * z));

        euler.x = CC_RADIANS_TO_DEGREES(euler.x);
        euler.y = CC_RADIANS_TO_DEGREES(euler.y);
        euler.z = -CC_RADIANS_TO_DEGREES(euler.z);
        return euler;
    }


    typedef cocos2d::Vec3(*AlignMethod)(const cocos2d::Rect &boundary, const cocos2d::Rect &rc);

    static void doAlign(EditorMultiSelector *self, AlignMethod method)
    {
        cocos2d::Rect rect = self->getSelectionBoundary();
        for(cocos2d::Node *node : self->getSortedSelection())
        {
            cocos2d::Rect rc = nodeGetWorldRect(node);
            cocos2d::Vec3 direction = method(rect, rc);
            node->getParent()->getWorldToNodeTransform().transformVector(&direction);

            cocos2d::Vec3 position = node->getPosition3D() + direction;
            node->setPosition3D(position);
            Framework::instance()->emitNodePropertyChange(node, "position", position);
        }

        self->updateSelectionBoundary();
    }

    static cocos2d::Vec3 getAlignLeftDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(boundary.getMinX() - rc.getMinX(), 0.0f, 0.0f);
    }

    static cocos2d::Vec3 getAlignRightDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(boundary.getMaxX() - rc.getMaxX(), 0.0f, 0.0f);
    }

    static cocos2d::Vec3 getAlignHCenterDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(boundary.getMidX() - rc.getMidX(), 0.0f, 0.0f);
    }

    static cocos2d::Vec3 getAlignTopDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(0.0f, boundary.getMaxY() - rc.getMaxY(), 0.0f);
    }

    static cocos2d::Vec3 getAlignBottomDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(0.0f, boundary.getMinY() - rc.getMinY(), 0.0f);
    }

    static cocos2d::Vec3 getAlignVCenterDir(const cocos2d::Rect &boundary, const cocos2d::Rect &rc)
    {
        return cocos2d::Vec3(0.0f, boundary.getMidY() - rc.getMidY(), 0.0f);
    }


    EditorMultiSelector::EditorMultiSelector()
    {
        setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));
    }

    void EditorMultiSelector::setAlignLeft()
    {
        doAlign(this, getAlignLeftDir);
    }

    void EditorMultiSelector::setAlignRight()
    {
        doAlign(this, getAlignRightDir);
    }

    void EditorMultiSelector::setAlignHCenter()
    {
        doAlign(this, getAlignHCenterDir);
    }

    void EditorMultiSelector::setAlignTop()
    {
        doAlign(this, getAlignTopDir);
    }

    void EditorMultiSelector::setAlignBottom()
    {
        doAlign(this, getAlignBottomDir);
    }

    void EditorMultiSelector::setAlignVCenter()
    {
        doAlign(this, getAlignVCenterDir);
    }


    void EditorMultiSelector::setSameWidth()
    {
        float maxWidth = 0;
        for(cocos2d::Node *node : sortedSelections_)
        {
            maxWidth = std::max(maxWidth, node->getContentSize().width);
        }

        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Size size = node->getContentSize();
            size.width = maxWidth;
            node->setContentSize(size);
            Framework::instance()->emitNodePropertyChange(node, "size", node->getContentSize());
        }

        updateSelectionBoundary();
    }

    void EditorMultiSelector::setSameHeight()
    {
        float maxHeight = 0;
        for(cocos2d::Node *node : sortedSelections_)
        {
            maxHeight = std::max(maxHeight, node->getContentSize().height);
        }

        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Size size = node->getContentSize();
            size.height = maxHeight;
            node->setContentSize(size);
            Framework::instance()->emitNodePropertyChange(node, "size", node->getContentSize());
        }

        updateSelectionBoundary();
    }

    void EditorMultiSelector::setSameSize()
    {
        cocos2d::Size size;
        for(cocos2d::Node *node : sortedSelections_)
        {
            size.width = std::max(size.width, node->getContentSize().width);
            size.height = std::max(size.height, node->getContentSize().height);
        }

        for(cocos2d::Node *node : sortedSelections_)
        {
            node->setContentSize(size);
            Framework::instance()->emitNodePropertyChange(node, "size", node->getContentSize());
        }

        updateSelectionBoundary();
    }

    void EditorMultiSelector::setSameHorizontalGap()
    {
        cocos2d::Rect rect = getSelectionBoundary();

        float totalWidth = 0.0f;
        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Rect rc = nodeGetWorldRect(node);
            totalWidth += rc.size.width;
        }

        float space = (rect.size.width - totalWidth) / ((int)sortedSelections_.size() - 1);
        cocos2d::Rect firstRect = nodeGetWorldRect(sortedSelections_.front());
        if(almostEqual(rect.getMinX(), firstRect.getMinX(), 1.0f))
        {
            //从左向右排列
            float startX = rect.getMinX();
            for(cocos2d::Node *node : sortedSelections_)
            {
                cocos2d::Rect rc = nodeGetWorldRect(node);
                cocos2d::Vec3 direction(startX - rc.getMinX(), 0.0f, 0.0f);
                node->getParent()->getWorldToNodeTransform().transformVector(&direction);

                startX += rc.size.width + space;

                cocos2d::Vec3 position = node->getPosition3D() + direction;
                node->setPosition3D(position);
                Framework::instance()->emitNodePropertyChange(node, "position", position);
            }
        }
        else
        {
            //从右向左排列
            float startX = rect.getMaxX();
            for(cocos2d::Node *node : sortedSelections_)
            {
                cocos2d::Rect rc = nodeGetWorldRect(node);
                cocos2d::Vec3 direction(startX - rc.getMaxX(), 0.0f, 0.0f);
                node->getParent()->getWorldToNodeTransform().transformVector(&direction);

                startX -= (rc.size.width + space);

                cocos2d::Vec3 position = node->getPosition3D() + direction;
                node->setPosition3D(position);
                Framework::instance()->emitNodePropertyChange(node, "position", position);
            }
        }
        updateSelectionBoundary();
    }

    void EditorMultiSelector::setSameVerticalGap()
    {
        cocos2d::Rect rect = getSelectionBoundary();

        float totalHeight = 0.0f;
        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Rect rc = nodeGetWorldRect(node);
            totalHeight += rc.size.height;
        }

        float space = (rect.size.height - totalHeight) / ((int)sortedSelections_.size() - 1);
        cocos2d::Rect firstRect = nodeGetWorldRect(sortedSelections_.front());
        if(almostEqual(rect.getMinY(), firstRect.getMinY(), 1.0f))
        {
            // 从下向上
            float startY = rect.getMinY();
            for(cocos2d::Node *node : sortedSelections_)
            {
                cocos2d::Rect rc = nodeGetWorldRect(node);
                cocos2d::Vec3 direction(0.0f, startY - rc.getMinY(), 0.0f);
                node->getParent()->getWorldToNodeTransform().transformVector(&direction);

                startY += rc.size.height + space;

                cocos2d::Vec3 position = node->getPosition3D() + direction;
                node->setPosition3D(position);
                Framework::instance()->emitNodePropertyChange(node, "position", position);
            }
        }
        else
        {
            // 从上向下
            float startY = rect.getMaxY();
            for(cocos2d::Node *node : sortedSelections_)
            {
                cocos2d::Rect rc = nodeGetWorldRect(node);
                cocos2d::Vec3 direction(0.0f, startY - rc.getMaxY(), 0.0f);
                node->getParent()->getWorldToNodeTransform().transformVector(&direction);

                startY -= (rc.size.height + space);

                cocos2d::Vec3 position = node->getPosition3D() + direction;
                node->setPosition3D(position);
                Framework::instance()->emitNodePropertyChange(node, "position", position);
            }
        }

        updateSelectionBoundary();
    }

    void EditorMultiSelector::setSelection(const NodeVector &selections)
    {
        assert(selections.size() >= 2);
        selections_ = selections;

        //保证父控件排在最前面
        sortedSelections_ = selections;
        auto compare = [](cocos2d::Node *a, cocos2d::Node *b) -> bool{
            while(a && a != b)
            {
                a = a->getParent();
            }
            return a != b;
        };
        std::stable_sort(sortedSelections_.begin(), sortedSelections_.end(), compare);

        updateSelectionBoundary();
    }

    cocos2d::Rect EditorMultiSelector::getSelectionBoundary()
    {
        cocos2d::Rect boundary;
        bool isFirst = true;

        for(cocos2d::Node *node : selections_)
        {
            cocos2d::Rect rect;
            rect.origin = cocos2d::Vec2::ZERO;
            rect.size = node->getContentSize();

            cocos2d::AffineTransform localTransform = node->getNodeToWorldAffineTransform();
            rect = cocos2d::RectApplyAffineTransform(rect, localTransform);

            if(isFirst)
            {
                boundary = rect;
                isFirst = false;
            }
            else
            {
                boundary = boundary.unionWithRect(rect);
            }
        }
        return boundary;
    }

    void EditorMultiSelector::updateSelectionBoundary()
    {
        cocos2d::Rect boundary = getSelectionBoundary();
        // convert boundary to parent space.
        if(getParent())
        {
            cocos2d::AffineTransform transform = getParent()->getWorldToNodeAffineTransform();
            boundary = cocos2d::RectApplyAffineTransform(boundary, transform);
        }

        base_class::setRotation3D(cocos2d::Vec3::ZERO);
        base_class::setScale(1.0f);
        base_class::setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));

        base_class::setContentSize(boundary.size);
        base_class::setPosition(boundary.origin + getAnchorPointInPoints());

        Framework::instance()->emitNodePropertyChange(this, "rotation", getRotation3D());
        Framework::instance()->emitNodePropertyChange(this, "scale", getScale3D());
        Framework::instance()->emitNodePropertyChange(this, "size", getContentSize());
        Framework::instance()->emitNodePropertyChange(this, "position", getPosition3D());
    }

    void EditorMultiSelector::clearSelection()
    {
        selections_.clear();
    }

    void EditorMultiSelector::setPosition3D(const cocos2d::Vec3& position)
    {
        cocos2d::Vec3 delta = position - getPosition3D();
        if(getParent())
        {
            getParent()->getNodeToWorldTransform().transformVector(&delta);
        }

        std::vector<cocos2d::Vec3> oldPositions;
        for(cocos2d::Node *node : sortedSelections_)
        {
            oldPositions.push_back(nodeGetWorldPosition(node));
        }

        int i = 0;
        for(cocos2d::Node *node : sortedSelections_)
        {
            nodeSetWorldPosition(node, oldPositions[i] + delta);
            Framework::instance()->emitNodePropertyChange(node, "position", node->getPosition3D());
            ++i;
        }

        base_class::setPosition3D(position);
    }

    void EditorMultiSelector::setRotation3D(const cocos2d::Vec3& rotation)
    {
        cocos2d::Quaternion rotationDelta = euler2quat(rotation - getRotation3D());

        cocos2d::Vec3 center(getAnchorPointInPoints().x, getAnchorPointInPoints().y, 0.0f);
        getNodeToWorldTransform().transformPoint(&center);

        std::vector<cocos2d::Mat4> oldTransforms;
        for(cocos2d::Node *node : sortedSelections_)
        {
            oldTransforms.push_back(node->getParent()->getNodeToWorldTransform());
        }

        int i = 0;
        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Vec3 oldPosition;
            oldTransforms[i].transformPoint(node->getPosition3D(), &oldPosition);

            cocos2d::Quaternion oldRotation;
            oldTransforms[i].decompose(nullptr, &oldRotation, nullptr);
            oldRotation *= node->getRotationQuat();

            cocos2d::Vec3 position = center + rotationDelta * (oldPosition - center);
            nodeSetWorldPosition(node, position);
            Framework::instance()->emitNodePropertyChange(node, "position", node->getPosition3D());

            cocos2d::Quaternion rotation = rotationDelta * oldRotation;
            nodeSetWorldRotation(node, rotation);
            Framework::instance()->emitNodePropertyChange(node, "rotation", node->getRotation3D());

            ++i;
        }

        base_class::setRotation3D(rotation);
    }

    void EditorMultiSelector::setScale3D(const cocos2d::Vec3& scale)
    {
        cocos2d::Vec3 scaleDelta = scale / getScale3D();

        cocos2d::Vec3 center(getAnchorPointInPoints().x, getAnchorPointInPoints().y, 0.0f);
        getNodeToWorldTransform().transformPoint(&center);

        for(cocos2d::Node *node : sortedSelections_)
        {
            cocos2d::Vec3 localCenter = center;
            node->getParent()->getWorldToNodeTransform().transformPoint(&localCenter);

            cocos2d::Vec3 position = (node->getPosition3D() - localCenter) * scaleDelta + localCenter;
            node->setPosition3D(position);
            Framework::instance()->emitNodePropertyChange(node, "position", position);

            cocos2d::Vec3 localScale(node->getScaleX(), node->getScaleY(), node->getScaleZ());
            localScale = localScale * scaleDelta;

            node->setScaleX(localScale.x);
            node->setScaleY(localScale.y);
            node->setScaleZ(localScale.z);

            Framework::instance()->emitNodePropertyChange(node, "scale", localScale);
        }

        base_class::setScaleX(scale.x);
        base_class::setScaleY(scale.y);
        base_class::setScaleZ(scale.z);
    }

    void EditorMultiSelector::setColor4B(const cocos2d::Color4B& color)
    {
        for(cocos2d::Node *node : selections_)
        {
            node->setColor(cocos2d::Color3B(color));
            node->setOpacity(color.a);
        }
    }

    void EditorMultiSelector::setContentSize(const cocos2d::Size& size)
    {
    }

    cocos2d::Vec3 EditorMultiSelector::getScale3D() const
    {
        return cocos2d::Vec3(_scaleX, _scaleY, _scaleZ);
    }

#define DECLARE_SETTER(PROPERTY_NAME, METHOD_NAME, VALUE_TYPE) \
    DECLARE_PROPERTY_SETTER(EditorMultiSelector, PROPERTY_NAME, METHOD_NAME, VALUE_TYPE)

    DECLARE_SETTER(position, setPosition3D, cocos2d::Vec3)
    DECLARE_SETTER(scale, setScale3D, cocos2d::Vec3)
    DECLARE_SETTER(rotation, setRotation3D, cocos2d::Vec3)
    DECLARE_SETTER(size, setContentSize, cocos2d::Size)
    DECLARE_SETTER(color, setColor4B, cocos2d::Color4B)

#undef DECLARE_SETTER

    EditorMultiSelectorLoader::EditorMultiSelectorLoader()
    {
        REGISTER_PROPERTY_SETTER(position);
        REGISTER_PROPERTY_SETTER(scale);
        REGISTER_PROPERTY_SETTER(rotation);
        REGISTER_PROPERTY_SETTER(size);
        REGISTER_PROPERTY_SETTER(color);
    }

    cocos2d::Node* EditorMultiSelectorLoader::createObject(const JsonHandle &config)
    {
        return EditorMultiSelector::create();
    }
}
