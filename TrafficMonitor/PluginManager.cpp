#include "stdafx.h"
#include "PluginManager.h"
#include "Common.h"

CPluginManager::CPluginManager()
{
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
        //载入dll
        plugin_info.plugin_module = LoadLibrary(plugin_info.file_path.c_str());
        if (plugin_info.plugin_module == NULL)
        {
            plugin_info.state = PluginState::PS_MUDULE_LOAD_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //获取函数的入口地址
        pfTMPluginGetItemNum TMPluginGetItemNum = (pfTMPluginGetItemNum)::GetProcAddress(plugin_info.plugin_module, "TMPluginGetItemNum");
        pfTMPluginCreateInstance TMPluginCreateInstance = (pfTMPluginCreateInstance)::GetProcAddress(plugin_info.plugin_module, "TMPluginCreateInstance");
        plugin_info.MPluginInfoRequired = (pfTMPluginInfoRequired)::GetProcAddress(plugin_info.plugin_module, "TMPluginInfoRequired");
        if (TMPluginGetItemNum == NULL || TMPluginCreateInstance == NULL || plugin_info.MPluginInfoRequired == NULL)
        {
            plugin_info.state = PluginState::PS_FUNCTION_GET_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        int item_num = TMPluginGetItemNum();
        for (int i = 0; i < item_num; i++)
        {
            std::shared_ptr<IPluginItem> item = std::shared_ptr<IPluginItem>(TMPluginCreateInstance(i));
            plugin_info.plugin_items.push_back(item);
            m_plugins.push_back(item);
        }
    }
}

const std::vector<std::shared_ptr<IPluginItem>>& CPluginManager::GetPluginItems()
{
    return m_plugins;
}

const std::vector<CPluginManager::PluginInfo>& CPluginManager::GetPlugins()
{
    return m_modules;
}

std::shared_ptr<IPluginItem> CPluginManager::GetItemByName(const std::wstring& item_name)
{
    for (const auto& item : m_plugins)
    {
        if (item->GetPluginName() == item_name)
            return item;
    }
    return nullptr;
}
