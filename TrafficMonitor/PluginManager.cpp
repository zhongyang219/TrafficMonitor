#include "stdafx.h"
#include "PluginManager.h"
#include "Common.h"
#include "TrafficMonitor.h"

#define PLUGIN_UNSUPPORT_VERSION 0      //不被支持的插件版本

CPluginManager::CPluginManager()
{
}

CPluginManager::~CPluginManager()
{
    //卸载插件
    for (const auto& m : m_modules)
        FreeLibrary(m.plugin_module);
}

static wstring WcharArrayToWString(const wchar_t* str)
{
    if (str == nullptr)
        return wstring();
    else
        return wstring(str);
}

void CPluginManager::LoadPlugins()
{
    //从plugins目录下加载插件
    wstring plugin_dir;
    plugin_dir = CCommon::GetModuleDir() + L"plugins";
    vector<wstring> plugin_files;
    CCommon::GetFiles((plugin_dir + L"\\*.dll").c_str(), plugin_files);		//获取Plugins目录下所有的dll文件的文件名
    for (const auto& file : plugin_files)
    {
        //插件信息
        m_modules.push_back(PluginInfo());
        PluginInfo& plugin_info{ m_modules.back() };
        //插件dll的路径
        plugin_info.file_path = plugin_dir + file;
        //插件文件名
        std::wstring file_name{ file };
        if (!file_name.empty() && (file_name[0] == L'\\' || file_name[0] == L'/'))
            file_name = file_name.substr(1);
        //如果插件被禁用，则不加载插件
        if (theApp.m_cfg_data.plugin_disabled.Contains(file_name))
        {
            plugin_info.state = PluginState::PS_DISABLE;
            continue;
        }
        //载入dll
        plugin_info.plugin_module = LoadLibrary(plugin_info.file_path.c_str());
        if (plugin_info.plugin_module == NULL)
        {
            plugin_info.state = PluginState::PS_MUDULE_LOAD_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //获取函数的入口地址
        pfTMPluginGetInstance TMPluginGetInstance = (pfTMPluginGetInstance)::GetProcAddress(plugin_info.plugin_module, "TMPluginGetInstance");
        if (TMPluginGetInstance == NULL)
        {
            plugin_info.state = PluginState::PS_FUNCTION_GET_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //创建插件对象
        plugin_info.plugin = TMPluginGetInstance();
        if (plugin_info.plugin == nullptr)
            continue;
        //检查插件版本
        int version = plugin_info.plugin->GetAPIVersion();
        if (version <= PLUGIN_UNSUPPORT_VERSION)
        {
            plugin_info.state = PluginState::PS_VERSION_NOT_SUPPORT;
            continue;
        }
        //获取插件信息
        for (int i{}; i < ITMPlugin::TMI_MAX; i++)
        {
            ITMPlugin::PluginInfoIndex index{ static_cast<ITMPlugin::PluginInfoIndex>(i) };
            plugin_info.properties[index] = WcharArrayToWString(plugin_info.plugin->GetInfo(index));
        }

        //获取插件显示项目
        int index = 0;
        while (true)
        {
            IPluginItem* item = plugin_info.plugin->GetItem(index);
            if (item == nullptr)
                break;
            plugin_info.plugin_items.push_back(item);
            m_plugins.push_back(item);
            m_plguin_item_map[item] = plugin_info.plugin;
            index++;
        }
    }

    //初始化所有任务栏显示项目
    for (const auto& display_item : AllDisplayItems)
    {
        m_all_display_items_with_plugins.insert(display_item);
    }
    for (const auto& display_item : m_plugins)
    {
        m_all_display_items_with_plugins.insert(display_item);
    }
}

const std::vector<IPluginItem*>& CPluginManager::GetPluginItems() const
{
    return m_plugins;
}

const std::vector<CPluginManager::PluginInfo>& CPluginManager::GetPlugins() const
{
    return m_modules;
}

IPluginItem* CPluginManager::GetItemById(const std::wstring& item_id)
{
    for (const auto& item : m_plugins)
    {
        if (item->GetItemId() == item_id)
            return item;
    }
    return nullptr;
}

IPluginItem* CPluginManager::GetItemByIndex(int index)
{
    if (index >= 0 && index < static_cast<int>(m_plugins.size()))
        return m_plugins[index];
    return nullptr;
}

int CPluginManager::GetItemIndex(IPluginItem* item) const
{
    for (auto iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
    {
        if (*iter == item)
            return iter - m_plugins.begin();
    }
    return -1;
}

ITMPlugin* CPluginManager::GetPluginByItem(IPluginItem* pItem)
{
    if (pItem == nullptr)
        return nullptr;
    return m_plguin_item_map[pItem];
}

const std::set<CommonDisplayItem>& CPluginManager::AllDisplayItemsWithPlugins()
{
    return m_all_display_items_with_plugins;
}

std::wstring CPluginManager::PluginInfo::Property(ITMPlugin::PluginInfoIndex index) const
{
    auto iter = properties.find(index);
    if (iter != properties.end())
        return iter->second;
    return wstring();
}
