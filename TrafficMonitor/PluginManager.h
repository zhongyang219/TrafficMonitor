#pragma once
#include "PluginInterface.h"
#include <memory>

typedef IPluginItem* (*pfTMPluginCreateInstance)(int);
typedef void (*pfTMPluginInfoRequired)();
typedef void (*pfTMPluginOptions)(HWND);
typedef const wchar_t* (*pfTMPluginGetInfo)(int);

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
        pfTMPluginOptions TMPluginOptions{};            //模块中TMPluginOptions函数的指针
        wstring name;
        wstring description;
        wstring author;
        wstring copyright;
    };

    CPluginManager();
    void LoadPlugins();

    const std::vector<std::shared_ptr<IPluginItem>>& GetPluginItems();
    const std::vector<PluginInfo>& GetPlugins();
    std::shared_ptr<IPluginItem> GetItemByName(const std::wstring& item_name);

private:
    std::vector<std::shared_ptr<IPluginItem>> m_plugins;
    std::vector<PluginInfo> m_modules;
};
