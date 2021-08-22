#include "pch.h"
#include <wchar.h>
#include "PluginSystemDate.h"
#include "DataManager.h"

const wchar_t* CPluginSystemDate::GetPluginName() const
{
    return L"Date";
}

const wchar_t* CPluginSystemDate::GetItemLableText() const
{
    return L"日期";
}

const wchar_t* CPluginSystemDate::GetItemValueText() const
{
    return CDataManager::Instance().m_cur_date.c_str();
}

const wchar_t* CPluginSystemDate::GetItemValueSampleText() const
{
    return L"2022/08/08";
}
