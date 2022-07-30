#pragma once
#include <shellscalingapi.h> // °üº¬::GetDpiForMonitor

typedef HRESULT(WINAPI* _GetDpiForMonitor)(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

class CDllFunctions
{
public:
    CDllFunctions();
    ~CDllFunctions();

public:
    HRESULT GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY);

private:
    _GetDpiForMonitor m_getDpiForMonitor{};

private:
    HMODULE m_shcore_module{};
};

