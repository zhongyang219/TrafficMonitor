#pragma once
class CWindowsSettingHelper
{
public:
    static bool IsWindows10LightTheme();				//判断Windows是否为浅色主题
    static void CheckWindows10LightTheme();
    static bool IsDotNetFramework4Point5Installed();	//判断是否安装了.Net Framework 4.5 (https://docs.microsoft.com/zh-cn/dotnet/framework/migration-guide/how-to-determine-which-versions-are-installed)

private:
    static LONG GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData);
    static bool GetDWORDRegKeyData(HKEY keyParent, const wstring& strKeyName, const wstring& strValueName, DWORD& dwValueData);

private:
    static 	bool m_light_theme;
};

