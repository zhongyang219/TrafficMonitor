#include "pch.h"
#include <wchar.h>
#include "PluginSystemTime.h"
#include "DataManager.h"

CPluginSystemTime::CPluginSystemTime()
{
}

const wchar_t* CPluginSystemTime::GetItemName() const
{
    return CDataManager::Instance().StringRes(IDS_TIME);
}

const wchar_t* CPluginSystemTime::GetItemId() const
{
    return L"ra1YX2g1";
}

const wchar_t* CPluginSystemTime::GetItemLableText() const
{
    return CDataManager::Instance().StringRes(IDS_TIME);
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
