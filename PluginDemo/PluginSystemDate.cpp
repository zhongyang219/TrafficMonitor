#include "pch.h"
#include <wchar.h>
#include "PluginSystemDate.h"

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
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);
    static wchar_t buff[128];
    swprintf_s(buff, L"%d/%.2d/%.2d", system_time.wYear, system_time.wMonth, system_time.wDay);
    return buff;
}

const wchar_t* CPluginSystemDate::GetItemValueSampleText() const
{
    return L"2022/08/08";
}
