#include "stdafx.h"
#include "WindowsSettingHelper.h"
#include "TrafficMonitor.h"

bool CWindowsSettingHelper::m_light_theme{};
bool CWindowsSettingHelper::IsWindows10LightTheme()
{
    return m_light_theme;
}

void CWindowsSettingHelper::CheckWindows10LightTheme()
{
    if (theApp.m_win_version.IsWindows10OrLater())
    {
        HKEY hKey;
        DWORD dwThemeData(0);
        LONG lRes = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_READ, &hKey);
        if (lRes == ERROR_SUCCESS) {
            GetDWORDRegKeyData(hKey, L"SystemUsesLightTheme", dwThemeData);
            m_light_theme = (dwThemeData != 0);
        }
        else
        {
            m_light_theme = false;
        }
        RegCloseKey(hKey);
    }
    else
    {
        m_light_theme = false;
    }
}

bool CWindowsSettingHelper::IsDotNetFramework4Point5Installed()
{
    DWORD netFramewordRelease{};
    if (!GetDWORDRegKeyData(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full", L"Release", netFramewordRelease))
        return false;
    return netFramewordRelease >= 379893;
}

bool CWindowsSettingHelper::IsTaskbarWidgetsBtnShown()
{
    if (!theApp.m_taskbar_data.is_windows_web_experience_detected)
        return false;
    DWORD data{};
    if (!GetDWORDRegKeyData(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarDa", data))
        return true;
    return data != 0;
}

bool CWindowsSettingHelper::IsTaskbarCenterAlign()
{
    DWORD data{};
    if (!GetDWORDRegKeyData(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", data))
        return true;
    return data != 0;
}

LONG CWindowsSettingHelper::GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData)
{
    DWORD dwBufferSize(sizeof(DWORD));
    DWORD dwResult(0);
    LONG lError = ::RegQueryValueExW(hKey, strValueName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&dwResult), &dwBufferSize);
    if (lError == ERROR_SUCCESS)
        dwValueData = dwResult;
    return lError;
}

bool CWindowsSettingHelper::GetDWORDRegKeyData(HKEY keyParent, const wstring& strKeyName, const wstring& strValueName, DWORD& dwValueData)
{
    CRegKey key;
    if (key.Open(keyParent, strKeyName.c_str(), KEY_READ) != ERROR_SUCCESS)
        return false;
    return (key.QueryDWORDValue(strValueName.c_str(), dwValueData) == ERROR_SUCCESS);
}
