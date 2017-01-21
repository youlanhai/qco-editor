#如何扩展新的控件
------------------
编辑器对扩展开放，可以不修改编辑器的任何代码以及配置文件就可完成扩展。如果是编辑器开发，必须按照原有的框架进行扩展。
以下以类Test为例，Test类结构如下：

    class Test : public Node
    {
    public:
        Test();
        void setID(int);
        void setName(const std::string&);
    };

##1.编写属性设置类
a.添加类`TestLoader`，并从`NodeLoader`派生，重写`createObject`方法，用于创建Test类型实例。头文件如下：

    class TestLoader : public NodeLoader
    {
    public:
        DECLARE_LOADER(TestLoader); //定义一些通用函数
        TestLoader(); //构造函数
        cocos2d::Node* createObject(const JsonHandle &config);//创建Test实例
    };

b.编写属性设置函数。比如设置id方法名为`test_set_id`，则函数体如下：

    void test_set_id(Test *p, const JsonHandle &value)
    {
        int v;
        value >> v;
        p->setID(v;
    }

c.在TestLoader类的构造函数中，向方法表中注册设置函数：
    `registerPropertySetter("id", (PropertySetter)test_set_id);`

d.将属性设置类注册到LoaderMgr。注册方法如下：
    `LoaderMgr::instance()->registerLoader("Test", new TestLoader());`

##2.注册C++类
将C++类型名绑定到用户定义的名称：
    `PropertyTreeMgr::instance()->registerUIName(typeid(Test).name(), "Test");`

##3.编写属性描述文件
属性描述文件用于告诉编辑器如何加载某类型的属性。你可以直接修改编辑器的默认配置文件，或者通过下面提供的方法创建额外的文件，然后手动注册到编辑器中。

a.属性值文件[非必须]
编辑器默认支持int，float，point，enum等属性，如果需要给这些类型设置附加参数，就需要值类型描述。具体范例参见`property/values.json`文件的写法，然后通过下面的方法进行注册：
    `PropertyItemFactory::instance()->registerProertyTemplate(valueFile)`

b.默认值文件[必须]
编辑只保存用户修改过的属性，这样不但能减少配置文件的大小，还能提高加载速度。默认值的数据必须和引擎中各属性的默认值保持相同，具体写法参见`property/default_value.json`，然后通过下面的方法进行注册：
    `PropertyDefault::instance()->loadDefault(defaultProperty)`

c.属性类型文件[必须]
编辑器根据属性类型文件，动态的切换属性UI界面。具体写法参见`property/properties.json`文件中的写法，然后通过下面的方法进行注册：
    `PropertyTreeMgr::instance()->loadPropertyFile(propertyFile)`

##4.将类型注册到编辑器的create子菜单中
修改`property/toolbox.json`文件。`menu`下的所有内容将显示在菜单中；`parameter`下的内容为创建类型实例的时候，所需要的默认值。
