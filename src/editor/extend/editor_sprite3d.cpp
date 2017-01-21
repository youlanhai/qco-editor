#include "editor_sprite3d.h"

#include <physics3d/CCPhysics3D.h>

USING_NS_CC;

EditorSprite3D::EditorSprite3D()
{

}

EditorSprite3D::~EditorSprite3D()
{

}

/*static*/ EditorSprite3D* EditorSprite3D::create()
{
    EditorSprite3D *p = new (std::nothrow) EditorSprite3D();
    if(p->init())
    {
        p->autorelease();
        return p;
    }
    delete p;
    return nullptr;
}

/*static*/ EditorSprite3D* EditorSprite3D::create(const std::string &resource)
{
    EditorSprite3D *sprite = new (std::nothrow) EditorSprite3D();
    if (sprite && sprite->initWithFile(resource))
    {
//        cocos2d::Size size = sprite->getBoundingBox().size;
//        sprite->_contentSize.setSize(std::max(0.0f, size.width), std::max(0.0f, size.height));

        AABB aabb = sprite->getAABB();
        Vec3 extend = aabb._max - aabb._min;

        Physics3DRigidBodyDes des;
        des.shape = Physics3DShape::createBox(extend);

        Physics3DRigidBody *rigidBody = Physics3DRigidBody::create(&des);
        rigidBody->setUserData(sprite);

        Physics3DComponent *componet = Physics3DComponent::create(rigidBody);
        componet->setSyncFlag(Physics3DComponent::PhysicsSyncFlag::NODE_TO_PHYSICS);
        sprite->addComponent(componet);

        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool EditorSprite3D::load(const std::string &resource)
{
    return initWithFile(resource);
}

