// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "PluginSystemDate.h"
#include "PluginSystemTime.h"
#include "PluginDemo.h"

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

int TMPluginGetItemNum()
{
    return 2;
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
