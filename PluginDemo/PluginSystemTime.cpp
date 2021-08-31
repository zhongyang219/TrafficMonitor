#include "pch.h"
#include <wchar.h>
#include "PluginSystemTime.h"
#include "DataManager.h"

const wchar_t* CPluginSystemTime::GetItemName() const
{
    return L"Time";
}

const wchar_t* CPluginSystemTime::GetItemId() const
{
    return L"ra1YX2g1";
}

const wchar_t* CPluginSystemTime::GetItemLableText() const
{
    if (CDataManager::Instance().m_setting_data.show_label_text)
        return L"时间";
    else
        return L"";
}

const wchar_t* CPluginSystemTime::GetItemValueText() const
{
    return CDataManager::Instance().m_cur_time.c_str();
}

const wchar_t* CPluginSystemTime::GetItemValueSampleText() const
{
    if (CDataManager::Instance().m_setting_data.show_second)
        return L"12:00:00";
    else
        return L"12:00";
}
