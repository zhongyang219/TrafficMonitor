#include "stdafx.h"
#include "DllFunctions.h"

CDllFunctions::CDllFunctions()
{
    //shellscalingapi
    m_shcore_module = ::LoadLibrary(_T("Shcore.dll"));
    if (m_shcore_module != NULL)
    {
        m_getDpiForMonitor = (_GetDpiForMonitor)::GetProcAddress(m_shcore_module, "GetDpiForMonitor");
    }
}

CDllFunctions::~CDllFunctions()
{
    if (m_shcore_module != NULL)
    {
        FreeLibrary(m_shcore_module);
        m_shcore_module = NULL;
    }
}

HRESULT CDllFunctions::GetDpiForMonitor(HMONITOR hmonitor, MONITOR_DPI_TYPE dpiType, UINT* dpiX, UINT* dpiY)
{
    if (m_getDpiForMonitor != nullptr)
        return m_getDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
    return 0;
}
