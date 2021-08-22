#include "pch.h"
#include <wchar.h>
#include "PluginSystemTime.h"

const wchar_t* CPluginSystemTime::GetPluginName() const
{
    return L"Time";
}

const wchar_t* CPluginSystemTime::GetItemLableText() const
{
    return L"时间";
}

const wchar_t* CPluginSystemTime::GetItemValueText() const
{
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);
    static wchar_t buff[128];
    swprintf_s(buff, L"%.2d:%.2d:%.2d", system_time.wHour, system_time.wMinute, system_time.wSecond);
    return buff;
}

const wchar_t* CPluginSystemTime::GetItemValueSampleText() const
{
    return L"12:00:00";
}
