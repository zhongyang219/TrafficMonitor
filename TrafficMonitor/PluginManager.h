#pragma once
#include "PluginInterface.h"
#include <map>
#include "TaskbarItemOrderHelper.h"
#include <functional>

typedef ITMPlugin* (*pfTMPluginGetInstance)();

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
        PS_VERSION_NOT_SUPPORT, //插件版本不被支持
        PS_DISABLE              //已禁用
    };

    //插件信息
    struct PluginInfo
    {
        wstring file_path;      //文件路径
        HMODULE plugin_module{};  //dll module
        ITMPlugin* plugin{};      //插件对象
        std::vector<IPluginItem*> plugin_items; //插件提供的所有显示项目
        PluginState state{};    //插件的状态
        DWORD error_code{};     //错误代码（GetLastError的返回值）
        std::map<ITMPlugin::PluginInfoIndex, std::wstring> properties;    //插件属性
        std::wstring Property(ITMPlugin::PluginInfoIndex) const;
    };

    CPluginManager();
    ~CPluginManager();
    void LoadPlugins();

    const std::vector<IPluginItem*>& GetPluginItems() const;
    const std::vector<PluginInfo>& GetPlugins() const;
    IPluginItem* GetItemById(const std::wstring& item_id);
    IPluginItem* GetItemByIndex(int index);
    int GetItemIndex(IPluginItem* item) const;
    ITMPlugin* GetPluginByItem(IPluginItem* pItem);
    int GetPluginIndex(ITMPlugin* plugin) const;

    //遍历所有插件
    //func: 参数为遍历到的ITMPlugin对象
    void EnumPlugin(std::function<void(ITMPlugin*)> func) const;

    //遍历所有插件项目
    //func: 参数为遍历到的IPluginItem对象
    void EnumPluginItem(std::function<void(IPluginItem*)> func) const;

    const std::set<CommonDisplayItem>& AllDisplayItemsWithPlugins();

    int GetItemWidth(IPluginItem* pItem, CDC* pDC);

private:
    static void ReplacePluginDrawTextFunction(HMODULE plgin_dll_module) noexcept;
    static void ReplaceMfcDrawTextFunction() noexcept;

private:
    std::vector<IPluginItem*> m_plugins;
    std::vector<PluginInfo> m_modules;
    std::set<CommonDisplayItem> m_all_display_items_with_plugins;   //包含插件在内的所有任务栏显示项目
    std::map<IPluginItem*, ITMPlugin*> m_plguin_item_map;          //用于根据插件项目查找对应插件的map
};
