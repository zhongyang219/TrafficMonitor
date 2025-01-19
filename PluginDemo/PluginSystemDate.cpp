#include "pch.h"
#include <wchar.h>
#include "PluginSystemDate.h"
#include "DataManager.h"

CPluginSystemDate::CPluginSystemDate()
{
}

const wchar_t* CPluginSystemDate::GetItemName() const
{
    return CDataManager::Instance().StringRes(IDS_DATE);
}

const wchar_t* CPluginSystemDate::GetItemId() const
{
    return L"uQI0sH6a";
}

const wchar_t* CPluginSystemDate::GetItemLableText() const
{
    return CDataManager::Instance().StringRes(IDS_DATE);
}

const wchar_t* CPluginSystemDate::GetItemValueText() const
{
    return CDataManager::Instance().m_cur_date.c_str();
}

const wchar_t* CPluginSystemDate::GetItemValueSampleText() const
{
    return L"2022/08/08";
}
