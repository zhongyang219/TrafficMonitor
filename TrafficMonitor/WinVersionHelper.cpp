#include "stdafx.h"
#include "WinVersionHelper.h"
#include "WindowsSettingHelper.h"


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

    CWindowsSettingHelper::CheckWindows10LightTheme();
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

bool CWinVersionHelper::IsWindows8Point1OrLater() const
{
	if (m_major_version > 6)
    {
        return true;
    }
    else if (m_major_version == 6 && m_minor_version >= 3)
    {
        return true;
    }
    return false;
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
