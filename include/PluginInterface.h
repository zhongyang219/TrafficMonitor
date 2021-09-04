/*********************************************************
* TrafficMonitor 插件接口
* Copyright (C) by Zhong Yang 2021
* zhongyang219@hotmail.com
**********************************************************/
#pragma once

//插件显示项目的接口
class IPluginItem
{
public:
    //获取显示项目的名称
    virtual const wchar_t* GetItemName() const = 0;

    //获取显示项目的唯一ID
    virtual const wchar_t* GetItemId() const = 0;

    //获取项目标签的文本
    virtual const wchar_t* GetItemLableText() const = 0;

    //获取项目数值的文本
    //由于此函数可能会被频繁调用，因此不要在这里获取监控数据，
    //而是在ITMPlugin::DataRequired函数中获取数据后保存起来，然后在这里返回获取的数值
    virtual const wchar_t* GetItemValueText() const = 0;

    //获取项目数值的示例文本
    //此函数返回的字符串的长度会用于计算显示区域的宽度
    virtual const wchar_t* GetItemValueSampleText() const = 0;

    //显示区域是否由插件自行绘制
    //如果返回false，则根据GetItemLableText和GetItemValueText返回的文本由主程序绘制显示区域，重写DrawItem函数将不起作用。
    //如果重写此函数并返回true，则必须重写DrawItem函数并在里面添加绘制显示区域的代码，
    //此时GetItemLableText、GetItemValueText和GetItemValueSampleText的返回值将被主程序忽略
    virtual bool IsCustomDraw() const { return false; }

    //获取显示区域的宽度
    //只有当CustomDraw()函数返回true时重写此函数才有效。
    //返回的值为DPI为96（100%）时的宽度，主程序会根据当前系统DPI的设置自动按比例放大，
    //因此你不需要为不同的DPI设置返回不同的值。
    //需要注意的是，这里的返回值代表了自绘区域所需要的最小宽度，DrawItem函数中的参数w的值可能会大于这个值
    virtual int GetItemWidth() const { return 0; }

    //自定义绘制显示区域的函数，只有当CustomDraw()函数返回true时重写此函数才有效
    //hDC: 绘图的上下文句柄
    //x,y,h,w: 绘图的矩形区域
    //dark_mode: 深色模式为true，浅色模式为false
    virtual void DrawItem(void* hDC, int x, int y, int w, int h, bool dark_mode) {}
};


//插件接口
class ITMPlugin
{
public:
    //插件接口的版本，仅当修改了插件接口时才会修改这里的返回值。
    //注意：插件开发者不应该修改这里的返回值，也不应该重写此虚函数。
    virtual int GetAPIVersion() const { return 1; }

    /*
    * 一个插件dll可以提供多个实现IPluginItem接口的对象，对应多个显示项目。
    * 参数index为对象的索引，当它的值大于或等于0且小于IPluginItem接口的对象的个数时，
    * 返回对象的IPluginItem接口的指针，其他情况应该返回空指针。
    * 例如插件提供两个显示项目，则当index等于0或1时返回对应IPluginItem接口的对象，
    * 其他值时必须返回空指针。
    */
    virtual IPluginItem* GetItem(int index) = 0;

    /*
    * 主程序会每隔一定时间调用此函数，插件需要在函数里获取一次监控的数据
    */
    virtual void DataRequired() = 0;

    //选项设置对话框的返回值
    enum OptionReturn
    {
        OR_OPTION_CHANGED,          //选项设置对话框中更改了选项设置
        OR_OPTION_UNCHANGED,        //选项设置对话框中未更改选项设置
        OR_OPTION_NOT_PROVIDED      //未提供选项设置对话框
    };

    /*
    * 主程序调用此函数以打开插件的选项设置对话框
    * hParent 为父窗口的句柄
    * 此函数不一定要重写。如果插件提供了选项设置界面，则应该重写此函数，并在最后返回OR_OPTION_CHANGED或OR_OPTION_UNCHANGED。
    * 返回值为OR_OPTION_NOT_PRVIDED则认为插件不提供选项设置对话框。
    */
    virtual OptionReturn ShowOptionsDialog(void* hParent) { return OR_OPTION_NOT_PROVIDED; };

    //插件信息的索引
    enum PluginInfoIndex
    {
        TMI_NAME,           //名称
        TMI_DESCRIPTION,    //描述
        TMI_AUTHOR,         //作者
        TMI_COPYRIGHT,      //版权
        TMI_VERSION,        //版本
        TMI_MAX             //插件信息的最大值
    };

    /*
    * 获取此插件的信息，根据index的值返回对应的信息
    */
    virtual const wchar_t* GetInfo(PluginInfoIndex index) = 0;

    //主程序的监控信息
    struct MonitorInfo
    {
        unsigned long long up_speed{};
        unsigned long long down_speed{};
        int cpu_usage{};
        int memory_usage{};
        int gpu_usage{};
        int hdd_usage{};
        int cpu_temperature{};
        int gpu_temperature{};
        int hdd_temperature{};
        int main_board_temperature{};
    };

    //主程序调用此函数以向插件传递所有获取到的监控信息
    virtual void OnMonitorInfo(const MonitorInfo& monitor_info) {};
};

/*
* 注意：插件dll需导出以下函数
* ITMPlugin* TMPluginGetInstance();
*   函数返回一个ITMPlugin接口的对象的指针。
*   此对象通常应该为一个全局或静态的对象，在程序运行结束前，它都不应该被释放。
*/
