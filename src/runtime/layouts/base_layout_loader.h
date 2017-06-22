#ifndef BASE_LAYOUT_LOADER_H
#define BASE_LAYOUT_LOADER_H

#include "../object_loader.h"
#include <CCRef.h>

// layout文件加载器基类
class PIP_DLL BaseLayoutLoader : public cocos2d::Ref
{
public:
    BaseLayoutLoader();
    virtual ~BaseLayoutLoader();

    virtual bool init();

    /** 获取当前文件加载器的版本 */
    virtual int getVersion() const = 0;

    /** 获取当前layout加载器支持的数据类型 */
    virtual const char* getType() const = 0;

    /** 从配置中加载对象。
     * @param config 	待加载对象的配置数据
     * @param p 		默认对象。如果为NULL，则根据配置数据，自动创建对象，并返回。否则，
     * 					将配置数据加载给p，并返回p。
     * @return 	如果p != NULL，则返回p。否则，返回新创建的对象。如果加载失败，均返回NULL。
    */
    virtual ObjectType* loadFromConfig(const JsonHandle &config, ObjectType *p) = 0;

    /** 将属性数据赋值给对象p。
     * @param loader 	对象数据加载器，用与将具体属性数据设置给对象p。
     * @param config 	对象的属性数据。
     * @param p 		待赋值的对象。
    */
    virtual void applyProperties(ObjectLoader *loader, const JsonHandle &config, ObjectType *p);

    /** 根据配置数据，选择对象加载器。*/
    ObjectLoader* choiceLoader(const JsonHandle &config);

};

#endif // BASE_LAYOUT_LOADER_H
