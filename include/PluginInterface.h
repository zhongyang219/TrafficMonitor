#pragma once
//插件接口的定义

class IPluginItem
{
public:
    //获取插件的名称
    virtual const wchar_t* GetPluginName() const = 0;

    //获取项目标签的文本
    virtual const wchar_t* GetItemLableText() const = 0;

    //获取项目数值的文本
    virtual const wchar_t* GetItemValueText() const = 0;

    //获取项目数值的示例文本
    virtual const wchar_t* GetItemValueSampleText() const = 0;
};

/*
* 注意：插件dll需要导出如下两个函数
* int TMPluginGetItemNum();
* IPluginItem* TMPluginCreateInstance(int index);
*
* 一个插件dll可以提供多个实现IPluginItem接口的对象，对应多个显示项目。
* TMPluginGetItemNum函数返回对象的个数。
* TMPluginCreateInstance函数用于创建对象，返回IPluginItem接口的指针，
* 参数index为对象的索引，它的值必须大于或等于0且小于TMPluginGetItemNum的返回值。
*/
