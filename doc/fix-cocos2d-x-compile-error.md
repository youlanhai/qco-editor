由于编译器的不同，在编译的过程中会报一些cocos2d-x的编译错。下面列举一些我碰到的问题，需要自己修改cocos2d-x的代码。

1. RefPtr编译错："T must be derived from Ref"。  
这是因为编辑器代码中使用了class前置声明，这些前置声明的代码，在编译时自然无法确定是Ref的派生类。注释掉`CCRefPtr.h`下面的这行static_assert代码：
```
static_assert(std::is_base_of<Ref, typename std::remove_const<T>::type>::value, "T must be derived from Ref");
```
2. `'static_cast': cannot convert from 'cocos2d::xxx *' to 'const cocos2d::Ref *'`  
这也是因为前置声明的的问题，编译器无法将前置声明的类转换为`cocos2d::Ref*`类型。修改`CCRefPtr.h`，将代码`const_cast<Ref*>(static_cast<const Ref*>(ptr))->retain/release()`，都替换成`((Ref*)ptr)->retain/release()`


