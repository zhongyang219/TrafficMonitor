#pragma once
class CWinVersionHelper
{
public:
	CWinVersionHelper();
	~CWinVersionHelper();

	bool IsWindows11OrLater() const;			//判断当前Windows版本是否为Win11或更新的版本
	bool IsWindows10FallCreatorOrLater() const;		//判断当前Windows版本是否为Win10秋季创意者更新或更新的版本
	bool IsWindows7() const;					//判断Windows版本是否为Windows7
	bool IsWindows8Or8point1() const;			//判断Windows版本是否为Windows8或Windows8.1
    bool IsWindows8OrLater() const;
    bool IsWindows10OrLater() const;
	bool IsWindows10LightTheme() const;				//判断Windows是否为浅色主题

	void CheckWindows10LightTheme();

	static bool IsDotNetFramework4Point5Installed();	//判断是否安装了.Net Framework 4.5 (https://docs.microsoft.com/zh-cn/dotnet/framework/migration-guide/how-to-determine-which-versions-are-installed)

	static LONG GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData);
	static bool GetDWORDRegKeyData(HKEY keyParent, const wstring& strKeyName, const wstring& strValueName, DWORD& dwValueData);

	int GetMajorVersion() const { return m_major_version; }
	int GetMinorVersion() const { return m_minor_version; }
	int GetBuildNumber() const { return m_build_number; }
protected:
	int m_major_version{};
	int m_minor_version{};
	int m_build_number{};
	bool m_light_theme{};
};
