#include "stdafx.h"
#include "WinVersionHelper.h"


CWinVersionHelper::CWinVersionHelper()
{
	DWORD dwMajorVer{}, dwMinorVer{}, dwBuildNumber{};
	HMODULE hModNtdll{};
	if (hModNtdll = ::LoadLibraryW(L"ntdll.dll"))
	{
		typedef void (WINAPI *pfRTLGETNTVERSIONNUMBERS)(DWORD*, DWORD*, DWORD*);
		pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
		pfRtlGetNtVersionNumbers = (pfRTLGETNTVERSIONNUMBERS)::GetProcAddress(hModNtdll, "RtlGetNtVersionNumbers");
		if (pfRtlGetNtVersionNumbers)
		{
			pfRtlGetNtVersionNumbers(&dwMajorVer, &dwMinorVer, &dwBuildNumber);
			dwBuildNumber &= 0x0ffff;
		}
		::FreeLibrary(hModNtdll);
		hModNtdll = NULL;
	}
	m_major_version = dwMajorVer;
	m_minor_version = dwMinorVer;
	m_build_number = dwBuildNumber;

	CheckWindows10LightTheme();
}


CWinVersionHelper::~CWinVersionHelper()
{
}

bool CWinVersionHelper::IsWindows11OrLater() const
{
	if (m_major_version > 10)
		return true;
	else if (m_major_version == 10 && m_minor_version > 0)
		return true;
	else if (m_major_version == 10 && m_minor_version == 0 && m_build_number >= 21996)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindows10FallCreatorOrLater() const
{
	if (m_major_version > 10)
		return true;
	else if (m_major_version == 10 && m_minor_version > 0)
		return true;
	else if (m_major_version == 10 && m_minor_version == 0 && m_build_number >= 16299)
		return true;
	else return false;
}

bool CWinVersionHelper::IsWindows7() const
{
	return (m_major_version == 6 && m_minor_version == 1);
}

bool CWinVersionHelper::IsWindows8Or8point1() const
{
	return (m_major_version == 6 && m_minor_version > 1);
}

bool CWinVersionHelper::IsWindows8OrLater() const
{
    if (m_major_version > 6)
        return true;
    else if (m_major_version == 6 && m_minor_version >= 2)
        return true;
    else return false;
}

bool CWinVersionHelper::IsWindows10OrLater() const
{
    return m_major_version >= 10;
}

bool CWinVersionHelper::IsWindows10LightTheme() const
{
	return m_light_theme;
}

void CWinVersionHelper::CheckWindows10LightTheme()
{
	if (m_major_version >= 10)
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

bool CWinVersionHelper::IsDotNetFramework4Point5Installed()
{
	DWORD netFramewordRelease{};
	if (!GetDWORDRegKeyData(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\NET Framework Setup\\NDP\\v4\\Full", L"Release", netFramewordRelease))
		return false;
	return netFramewordRelease >= 379893;
}

LONG CWinVersionHelper::GetDWORDRegKeyData(HKEY hKey, const wstring& strValueName, DWORD& dwValueData)
{
	DWORD dwBufferSize(sizeof(DWORD));
	DWORD dwResult(0);
	LONG lError = ::RegQueryValueExW(hKey, strValueName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&dwResult), &dwBufferSize);
	if (lError == ERROR_SUCCESS)
		dwValueData = dwResult;
	return lError;
}

bool CWinVersionHelper::GetDWORDRegKeyData(HKEY keyParent, const wstring& strKeyName, const wstring& strValueName, DWORD& dwValueData)
{
    CRegKey key;
	if (key.Open(keyParent, strKeyName.c_str(), KEY_READ) != ERROR_SUCCESS)
		return false;
	return (key.QueryDWORDValue(strValueName.c_str(), dwValueData) == ERROR_SUCCESS);
}
