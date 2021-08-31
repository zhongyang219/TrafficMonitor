#include "stdafx.h"
#include "PluginManager.h"
#include "Common.h"

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
        //载入dll
        plugin_info.plugin_module = LoadLibrary(plugin_info.file_path.c_str());
        if (plugin_info.plugin_module == NULL)
        {
            plugin_info.state = PluginState::PS_MUDULE_LOAD_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //获取函数的入口地址
        pfTMPluginCreateInstance TMPluginCreateInstance = (pfTMPluginCreateInstance)::GetProcAddress(plugin_info.plugin_module, "TMPluginCreateInstance");
        if (TMPluginCreateInstance == NULL)
        {
            plugin_info.state = PluginState::PS_FUNCTION_GET_FAILED;
            plugin_info.error_code = GetLastError();
            continue;
        }
        //创建插件对象
        plugin_info.plugin = std::shared_ptr<ITMPlugin>(TMPluginCreateInstance());
        if (plugin_info.plugin == nullptr)
            continue;
        //获取插件信息
        plugin_info.name = WcharArrayToWString(plugin_info.plugin->GetInfo(ITMPlugin::TMI_NAME));
        plugin_info.description = WcharArrayToWString(plugin_info.plugin->GetInfo(ITMPlugin::TMI_DESCRIPTION));
        plugin_info.author = WcharArrayToWString(plugin_info.plugin->GetInfo(ITMPlugin::TMI_AUTHOR));
        plugin_info.copyright = WcharArrayToWString(plugin_info.plugin->GetInfo(ITMPlugin::TMI_COPYRIGHT));

        //获取插件显示项目
        int index = 0;
        while (true)
        {
            IPluginItem* item = plugin_info.plugin->GetItem(index);
            if (item == nullptr)
                break;
            plugin_info.plugin_items.push_back(item);
            m_plugins.push_back(item);
            index++;
        }
    }
}

const std::vector<IPluginItem*>& CPluginManager::GetPluginItems()
{
    return m_plugins;
}

const std::vector<CPluginManager::PluginInfo>& CPluginManager::GetPlugins()
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
