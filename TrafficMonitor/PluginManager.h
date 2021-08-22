#pragma once
#include "PluginInterface.h"
#include <memory>

typedef int (*pfTMPluginGetItemNum)();
typedef IPluginItem* (*pfTMPluginCreateInstance)(int);
typedef void (*pfTMPluginInfoRequired)();

//用于加载和管理插件
class CPluginManager
{
public:
    //插件的状态
    enum class PluginState
    {
        PS_SUCCEED,             //载入成功
        PS_MUDULE_LOAD_FAILED,  //dll加载失败
        PS_FUNCTION_GET_FAILED, //插件函数获取失败
    };

    //插件信息
    struct PluginInfo
    {
        wstring file_path;      //文件路径
        HMODULE plugin_module;  //dll module
        std::vector<std::shared_ptr<IPluginItem>> plugin_items; //插件提供的所有显示项目
        PluginState state{};    //插件的状态
        DWORD error_code{};     //错误代码（GetLastError的返回值）
        pfTMPluginInfoRequired MPluginInfoRequired{};   //模块中MPluginInfoRequired函数的指针
    };

    CPluginManager();
    void LoadPlugins();

    const std::vector<std::shared_ptr<IPluginItem>>& GetPluginItems();
    const std::vector<PluginInfo>& GetPlugins();

private:
    std::vector<std::shared_ptr<IPluginItem>> m_plugins;
    std::vector<PluginInfo> m_modules;
};
