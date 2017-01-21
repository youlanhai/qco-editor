由于spine代码些的不规范，没有导出符号表，所以没法引用。如果要编译动态库，只能禁用spine相关代码了。

使用cmake编译cocos2d-x动态链接库，会有很多编译错误，需要手动修改cocos2d-x的相关代码。

1. 用Visual Studio 2015编译器时，需要修改cmake文件`cocos2d-x/cmake/Modules/CocosUsePrebuiltLibs.cmake`，链接上vs2015专属库。在代码`
foreach(_lib ${all_prebuilt_libs})`之前加入如下代码：
```
if(MSVC14)
  set(_png_libs libpng-2015)
  set(_jpeg_libs libjpeg-2015)
  set(_tiff_libs libtiff-2015)
  set(_glfw3_libs glfw3-2015)
  set(_chipmunk_libs libchipmunk-2015)
endif()
```
2. 修改`cocos2d-x/cocos/CMakeLists.txt`，加入Windows依赖库`version.lib`
```
list(APPEND PLATFORM_SPECIFIC_LIBS ws2_32 winmm version) # 在末尾加上version库
```
3. 修改`cocos2d-x/cocos/cc_dummy.c`，确保在windows平台上生成cocos2d.lib文件。
```
#ifdef _WIN32
void __declspec(dllexport) making_cmake_happy(void)
#else
static void making_cmake_happy(void)
#endif
{
}
```
4. 修改`cocos2d-x/cocos/scripting/lua-bindings/CMakeLists.txt`，在其中加上代码：
```
if(BUILD_SHARED_LIBS)
    # 导出符号
    add_definitions(-D_USRLUADLL)
    # 下面的这些宏是用于导出符号的，应该是每个模块私有宏，不应该加到全局模块中。这会导致依赖他的模块把宏解析成export而不是import，这里把他们删除掉。
    remove_definitions(-D_USRDLL -D_EXPORT_DLL_ -D_USEGUIDLL -D_USREXDLL -D_USRSTUDIODLL)
endif()
link_directories(
    ${cocos_root}/external/lua/luajit/prebuilt/win32
)
```

并把spine相关的代码文件都注释调。

5. 修改`cocos\scripting\lua-bindings\manual\controller\lua_cocos2dx_controller_manual.cpp
`，在开头加上代码`#include "platform/CCPlatformMacros.h"`

6. 修改`cocos\scripting\lua-bindings\manual\lua_module_register.cpp
`，注释掉spine相关的代码行：`register_spine_module(L);`。
