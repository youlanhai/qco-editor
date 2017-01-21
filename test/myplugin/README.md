自定义插件

# 添加编译阶段
```
comand: python
arguments: ../../bin/resolvelib.py -m lib -lib lib/libplugin-myplugin.dylib
directory: current path.
```

# 制作插件步骤
1. 创建qt lib项目。参照工程`myplugin.pro`
2. 从`PluginInterface`派生出插件类，定义插件接口  
`DEFINE_PLUGIN(myplugin, Editor::MyPlugin)`
3. 编译项目生成动态链接库，并处理库文件的依赖项  
`python ../../bin/resolvelib.py -m lib -lib lib/libplugin-myplugin.dylib`
4. 将生成的动态链接库文件（插件）拷贝到资源项目的`plugins`目录下。
