#include "stdafx.h"
#include "SkinManager.h"
#include "Common.h"
#include "TrafficMonitor.h"

CSkinManager CSkinManager::m_instance;

const std::wstring SKIN_SETTINGS_PREFIX = L"skin_";

void CSkinManager::Init()
{
    if (!m_init)    //防止重复加载
    {
        //获取所有皮肤名称
        CCommon::GetFiles((theApp.m_skin_path + L"*").c_str(), [&](const wstring& file_name)
            {
                if (CCommon::IsFolder(theApp.m_skin_path + file_name))
                    m_skins.push_back(file_name);
            });
        if (m_skins.empty())
            m_skins.push_back(L"");

        //载入所有皮肤的独立设置
        CIniHelper ini(theApp.m_config_path);
        std::vector<std::wstring> skin_data_app_names = ini.GetAllAppName(SKIN_SETTINGS_PREFIX);
        for (const std::wstring& skin_name : skin_data_app_names)
        {
            std::wstring app_name = SKIN_SETTINGS_PREFIX + skin_name;
            //判断皮肤是否存在
            if (std::find(m_skins.begin(), m_skins.end(), skin_name) != m_skins.end())
            {
                SkinSettingData data;
                //文本颜色
                ini.LoadMainWndColors(app_name.c_str(), L"text_color", data.text_colors, 16384); //根据皮肤是否存在来设置默认的文本颜色，皮肤文件不存在时文本颜色默认为白色
                //字体
                FontInfo default_font{};
                default_font.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
                default_font.size = 10;
                ini.LoadFontData(app_name.c_str(), data.font, default_font);
                //显示文本
                ini.LoadDisplayStr(app_name.c_str(), data.disp_str, true);

                m_skin_setting_data_map[skin_name] = data;
            }
        }
        m_init = true;
    }
}

CSkinManager& CSkinManager::Instance()
{
    return m_instance;
}

std::wstring CSkinManager::GetSkinName(int index) const
{
    if (index >= 0 && index < static_cast<int>(m_skins.size()))
        return m_skins[index];
    else
        return std::wstring();
}

int CSkinManager::FindSkinIndex(const std::wstring& skin_name) const
{
    std::wstring _skin_name{ skin_name };
    //如果名称前面有斜杠，则将它去掉
    if (!_skin_name.empty() && (_skin_name[0] == L'\\' || _skin_name[0] == L'/'))
    {
        _skin_name = _skin_name.substr(1);
    }
    for (size_t i{}; i < m_skins.size(); i++)
    {
        if (m_skins[i] == _skin_name)
            return static_cast<int>(i);
    }
    return 0;
}

bool CSkinManager::GetSkinSettingDataByIndex(int index, SkinSettingData& skin_data)
{
    std::wstring skin_name = GetSkinName(index);
    if (!skin_name.empty())
    {
        auto iter = m_skin_setting_data_map.find(skin_name);
        if (iter != m_skin_setting_data_map.end())
        {
            skin_data = iter->second;
            return true;
        }
    }
    return false;
}

const vector<wstring>& CSkinManager::GetSkinNames() const
{
    return m_skins;
}

int CSkinManager::Size() const
{
    return static_cast<int>(m_skins.size());
}

void CSkinManager::AddSkinSettingData(const std::wstring& skin_name, const SkinSettingData& data)
{
    m_skin_setting_data_map[skin_name] = data;
}

void CSkinManager::Save()
{
    //保存所有皮肤的独立配置
    CIniHelper ini(theApp.m_config_path);
    for (const auto& data : m_skin_setting_data_map)
    {
        std::wstring app_name = SKIN_SETTINGS_PREFIX + data.first;
        //文本颜色
        ini.SaveMainWndColors(app_name.c_str(), L"text_color", data.second.text_colors);
        //字体
        ini.SaveFontData(app_name.c_str(), data.second.font);
        //显示文本
        ini.SaveDisplayStr(app_name.c_str(), data.second.disp_str);
        ini.Save();
    }
}
