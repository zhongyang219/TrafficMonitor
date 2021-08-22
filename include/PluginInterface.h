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
    //由于此函数可能会被频繁调用，因此不要在这里获取监控数据，
    //而是在TMPluginInfoRequired函数中获取数据后保存起来，然后在这里返回获取的数值
    virtual const wchar_t* GetItemValueText() const = 0;

    //获取项目数值的示例文本
    virtual const wchar_t* GetItemValueSampleText() const = 0;
};

/*
* 注意：插件dll需要导出如下函数
*
* int TMPluginGetItemNum();
*   一个插件dll可以提供多个实现IPluginItem接口的对象，对应多个显示项目。
*   TMPluginGetItemNum函数返回对象的个数。
*
* IPluginItem* TMPluginCreateInstance(int index);
*   TMPluginCreateInstance函数用于创建对象，返回IPluginItem接口的指针，
*   参数index为对象的索引，它的值必须大于或等于0且小于TMPluginGetItemNum的返回值。
*
* void TMPluginInfoRequired();
*   主程序会每隔一定时间调用此函数，插件需要在函数里获取一次监控的数据
*/
