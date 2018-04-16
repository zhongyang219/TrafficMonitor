#include "stdafx.h"
#include "IniHelper.h"


CIniHelper::CIniHelper()
{
}


CIniHelper::~CIniHelper()
{
}

void CIniHelper::SetPath(const wstring & path)
{
	m_path = path;
}

bool CIniHelper::WriteString(const wchar_t * AppName, const wchar_t * KeyName, wstring str)
{
	str = DEF_CH + str + DEF_CH;
	return (::WritePrivateProfileStringW(AppName, KeyName, str.c_str(), m_path.c_str()) != FALSE);
}

wstring CIniHelper::GetString(const wchar_t * AppName, const wchar_t * KeyName, const wchar_t * default_str)
{
	wstring rtn;
	wchar_t buff[256];
	::GetPrivateProfileStringW(AppName, KeyName, default_str, buff, 256, m_path.c_str());
	rtn = buff;
	//如果读取的字符串前后有指定的字符，则删除它
	if (!rtn.empty() && (rtn.front() == L'$' || rtn.front() == DEF_CH))
		rtn = rtn.substr(1);
	if (!rtn.empty() && (rtn.back() == L'$' || rtn.back() == DEF_CH))
		rtn.pop_back();
	return rtn;
}

bool CIniHelper::WriteInt(const wchar_t * AppName, const wchar_t * KeyName, int value)
{
	wchar_t buff[11]{};
	_itow_s(value, buff, 10);
	return (::WritePrivateProfileStringW(AppName, KeyName, buff, m_path.c_str()) != FALSE);
}

int CIniHelper::GetInt(const wchar_t * AppName, const wchar_t * KeyName, int default_value)
{
	return GetPrivateProfileIntW(AppName, KeyName, default_value, m_path.c_str());
}

bool CIniHelper::WriteBool(const wchar_t * AppName, const wchar_t * KeyName, bool value)
{
	if(value)
		return (::WritePrivateProfileStringW(AppName, KeyName, L"1", m_path.c_str()) != FALSE);
	else
		return (::WritePrivateProfileStringW(AppName, KeyName, L"0", m_path.c_str()) != FALSE);
}

bool CIniHelper::GetBool(const wchar_t * AppName, const wchar_t * KeyName, bool default_value)
{
	return (GetPrivateProfileIntW(AppName, KeyName, default_value, m_path.c_str()) != 0);
}
