#ifndef COC_ANIMATION_MGR_H
#define COC_ANIMATION_MGR_H

#include "coc_config.h"
#include "runtime/singleton.h"

#include "runtime/smartpointer.h"
#include <unordered_map>

NS_COC_BEGIN
class Animation;
typedef cocos2d::SmartPointer<Animation> AnimationPtr;

class AnimationMgr : public Singleton<AnimationMgr>
{
public:
    AnimationMgr();
    ~AnimationMgr();
    
    /**
     * @brief 获取或加载动画。如果缓存中存在，直接返回。否则进行加载，并加入缓存。
     */
    Animation* getOrLoad(const std::string& path);

    Animation* find(const std::string& path);

    void purge(const std::string& path);
    void purge(Animation* am);

    /**
     * @brief 移除所有引用计数为1的Animation
     */
    void purgeUnused();

    /**
     * @brief 移除所有Animation
     */
    void purgeAll();

private:
    
    typedef std::unordered_map<std::string, AnimationPtr> AnimationMap;
    AnimationMap    cache_;
};
NS_COC_END

#endif // COC_ANIMATION_MGR_H
