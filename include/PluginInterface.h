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

    //获取项目标签的文本
    virtual const wchar_t* GetItemLableText() const = 0;

    //获取项目数值的文本
    //由于此函数可能会被频繁调用，因此不要在这里获取监控数据，
    //而是在ITMPlugin::DataRequired函数中获取数据后保存起来，然后在这里返回获取的数值
    virtual const wchar_t* GetItemValueText() const = 0;

    //获取项目数值的示例文本
    virtual const wchar_t* GetItemValueSampleText() const = 0;
};


//插件接口
class ITMPlugin
{
public:
    int GetAPIVersion() const { return 1; }

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

    /*
    * 主程序调用此函数以打开插件的选项设置对话框
    * hParent 为父窗口的句柄
    * 此函数不一定要重写。如果插件提供了选项设置界面，则应该重写此函数，并在最后返回true。
    * 返回值为false则认为插件不提供选项设置对话框。
    */
    virtual bool ShowOptionsDialog(HWND hParent) { return false; };

    //插件信息的索引
    enum PluginInfoIndex
    {
        TMI_NAME,           //名称
        TMI_DESCRIPTION,    //描述
        TMI_AUTHOR,         //作者
        TMI_COPYRIGHT       //版权
    };

    /*
    * 获取此插件的信息，根据index的值返回对应的信息
    */
    virtual const wchar_t* GetInfo(PluginInfoIndex index) = 0;
};

/*
* 注意：插件dll需导出以下函数
* ITMPlugin* TMPluginCreateInstance();
*   函数创建一个实现了ITMPlugin接口的对象，并返回其指针。
*   对象的释放由调用者来完成。
*/
