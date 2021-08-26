// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "PluginSystemDate.h"
#include "PluginSystemTime.h"
#include "PluginDemo.h"
#include "DataManager.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

IPluginItem* TMPluginCreateInstance(int index)
{
    switch (index)
    {
    case 0:
        return new CPluginSystemDate();
    case 1:
        return new CPluginSystemTime();
    default:
        break;
    }
    return nullptr;
}

void TMPluginInfoRequired()
{
    //获取时间和日期
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);
    wchar_t buff[128];
    swprintf_s(buff, L"%d/%.2d/%.2d", system_time.wYear, system_time.wMonth, system_time.wDay);
    CDataManager::Instance().m_cur_date = buff;
    swprintf_s(buff, L"%.2d:%.2d:%.2d", system_time.wHour, system_time.wMinute, system_time.wSecond);
    CDataManager::Instance().m_cur_time = buff;
}
