#include "coc_animation_mgr.h"
#include "coc_animation.h"

IMPLEMENT_SINGLETON(coc::AnimationMgr);

NS_COC_BEGIN
AnimationMgr::AnimationMgr()
{

}

AnimationMgr::~AnimationMgr()
{

}

Animation* AnimationMgr::getOrLoad(const std::string& path)
{
	Animation* ret = find(path);
	if(ret == nullptr)
	{
		ret = new Animation();
		if(!ret->loadFromFile(path))
		{
			delete ret;
			ret = nullptr;
		}
		else
		{
			cache_[path] = ret;
			ret->release();
		}
	}
	return ret;
}

Animation* AnimationMgr::find(const std::string& path)
{
	auto it = cache_.find(path);
	if(it != cache_.end())
	{
		return it->second;
	}
	return nullptr;
}

void AnimationMgr::purge(const std::string& path)
{
	auto it = cache_.find(path);
	if(it != cache_.end())
	{
        cache_.erase(it);
	}
}

void AnimationMgr::purge(Animation* am)
{
	for(auto it = cache_.begin(); it != cache_.end(); ++it)
	{
		if(am == it->second)
		{
			cache_.erase(it);
			return;
		}
	}
}

void AnimationMgr::purgeUnused()
{
	for(auto it = cache_.begin(); it != cache_.end(); /*do nothing*/)
	{
        if(it->second->getReferenceCount() == 1)
		{
            CCLOG("CocAnimation purge: %s", it->first.c_str());
			it = cache_.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void AnimationMgr::purgeAll()
{
	cache_.clear();
}

NS_COC_END
