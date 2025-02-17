#include "stdafx.h"
#include "DllFunctions.h"

CDllFunctions::CDllFunctions()
{
    // shellscalingapi
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
    return E_NOINTERFACE;
}

#define TRAFFICMONITOR_DEFINE_STATIC_MEMBER_IN_DLL_FUNCTIONS(member_name, ...) \
    decltype(CDllFunctions::member_name) CDllFunctions::member_name(__VA_ARGS__)

TRAFFICMONITOR_DEFINE_STATIC_MEMBER_IN_DLL_FUNCTIONS(
    D3DCompile,
    _T("d3dcompiler_47.dll"), "D3DCompile");

TRAFFICMONITOR_DEFINE_STATIC_MEMBER_IN_DLL_FUNCTIONS(
    DCompositionCreateDevice,
    _T("dcomp.dll"), "DCompositionCreateDevice");

TRAFFICMONITOR_DEFINE_STATIC_MEMBER_IN_DLL_FUNCTIONS(
    CreateDXGIFactory2,
    _T("dxgi.dll"), "CreateDXGIFactory2");