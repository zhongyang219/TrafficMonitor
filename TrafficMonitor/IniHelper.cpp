#include "stdafx.h"
#include "IniHelper.h"


CIniHelper::CIniHelper(const wstring& file_path)
{
	m_file_path = file_path;
	ifstream file_stream{ file_path };
	if (file_stream.fail())
	{
		return;
	}
	//读取文件内容
	string ini_str;
	while (!file_stream.eof())
	{
		ini_str.push_back(file_stream.get());
	}
	ini_str.pop_back();
	if (!ini_str.empty() && ini_str.back() != L'\n')		//确保文件末尾有回车符
		ini_str.push_back(L'\n');
	//判断文件是否是utf8编码
	bool is_utf8;
	if (ini_str.size() >= 3 && ini_str[0] == -17 && ini_str[1] == -69 && ini_str[2] == -65)
	{
		//如果有UTF8的BOM，则删除BOM
		is_utf8 = true;
		ini_str = ini_str.substr(3);
	}
	else
	{
		is_utf8 = false;
	}
	//转换成Unicode
	m_ini_str = CCommon::StrToUnicode(ini_str.c_str(), is_utf8);
}


CIniHelper::~CIniHelper()
{
}

void CIniHelper::SetSaveAsUTF8(bool utf8)
{
	m_save_as_utf8 = utf8;
}

void CIniHelper::WriteString(const wchar_t * AppName, const wchar_t * KeyName, const wstring& str)
{
	wstring write_str{ str };
	if (!write_str.empty() && (write_str[0] == L' ' || write_str.back() == L' '))		//如果字符串前后含有空格，则在字符串前后添加引号
	{
		write_str = DEF_CH + write_str;
		write_str.push_back(DEF_CH);
	}
	_WriteString(AppName, KeyName, write_str);
}

wstring CIniHelper::GetString(const wchar_t * AppName, const wchar_t * KeyName, const wchar_t* default_str) const
{
	wstring rtn{_GetString(AppName, KeyName, default_str)};
	//如果读取的字符串前后有指定的字符，则删除它
	if (!rtn.empty() && (rtn.front() == L'$' || rtn.front() == DEF_CH))
		rtn = rtn.substr(1);
	if (!rtn.empty() && (rtn.back() == L'$' || rtn.back() == DEF_CH))
		rtn.pop_back();
	return rtn;
}

void CIniHelper::WriteInt(const wchar_t * AppName, const wchar_t * KeyName, int value)
{
	wchar_t buff[16]{};
	_itow_s(value, buff, 10);
	_WriteString(AppName, KeyName, wstring(buff));
}

int CIniHelper::GetInt(const wchar_t * AppName, const wchar_t * KeyName, int default_value) const
{
	wchar_t default_str_buff[16]{};
	_itow_s(default_value, default_str_buff, 10);
	wstring rtn{ _GetString(AppName, KeyName, default_str_buff) };
	return _ttoi(rtn.c_str());
}

void CIniHelper::WriteBool(const wchar_t * AppName, const wchar_t * KeyName, bool value)
{
	if(value)
		_WriteString(AppName, KeyName, wstring(L"true"));
	else
		_WriteString(AppName, KeyName, wstring(L"false"));
}

bool CIniHelper::GetBool(const wchar_t * AppName, const wchar_t * KeyName, bool default_value) const
{
	wstring rtn{ _GetString(AppName, KeyName, (default_value ? L"true" : L"false")) };
	if (rtn == L"true")
		return true;
	else if (rtn == L"false")
		return false;
	else
		return (_ttoi(rtn.c_str()) != 0);
}

void CIniHelper::WriteIntArray(const wchar_t * AppName, const wchar_t * KeyName, const int * values, int size)
{
	CString str, tmp;
	for (int i{}; i < size; i++)
	{
		tmp.Format(_T("%d,"), values[i]);
		str += tmp;
	}
	_WriteString(AppName, KeyName, wstring(str));
}

void CIniHelper::GetIntArray(const wchar_t * AppName, const wchar_t * KeyName, int * values, int size, int default_value) const
{
	CString default_str;
	default_str.Format(_T("%d"), default_value);
	wstring str;
	str = _GetString(AppName, KeyName, default_str);
	size_t index{}, index0{};
	for (int i{}; i < size; i++)
	{
		index0 = index;
		if (index0 < 256 && str[index0] == L',')
			index0++;
		index = str.find(L',', index + 1);
		if (index0 == index)
		{
			if(i!=0)
				values[i] = values[i - 1];		//如果后面已经没有数据，则填充为前一个数据
			else
				values[i] = default_value;
		}
		else
		{
			wstring tmp = str.substr(index0, index - index0);
			values[i] = _wtoi(tmp.c_str());
		}
	}
}

void CIniHelper::WriteBoolArray(const wchar_t * AppName, const wchar_t * KeyName, const bool * values, int size)
{
	int value{};
	for (int i{}; i < size; i++)
	{
		if (values[i])
			value |= (1 << i);
	}
	return WriteInt(AppName, KeyName, value);
}

void CIniHelper::GetBoolArray(const wchar_t * AppName, const wchar_t * KeyName, bool * values, int size, bool default_value) const
{
	int value = GetInt(AppName, KeyName, 0);
	for (int i{}; i < size; i++)
	{
		values[i] = ((value >> i) % 2 != 0);
	}
}

void CIniHelper::SaveFontData(const wchar_t * AppName, const FontInfo & font)
{
	WriteString(AppName, L"font_name", wstring(font.name));
	WriteInt(AppName, L"font_size", font.size);
	bool style[4];
	style[0] = font.bold;
	style[1] = font.italic;
	style[2] = font.underline;
	style[3] = font.strike_out;
	WriteBoolArray(AppName, L"font_style", style, 4);
}

bool CIniHelper::Save()
{
	ofstream file_stream{ m_file_path };
	if(file_stream.fail())
		return false;
	string ini_str{ CCommon::UnicodeToStr(m_ini_str.c_str(), m_save_as_utf8) };
	if (m_save_as_utf8)		//如果以UTF8编码保存，先插入BOM
	{
		string utf8_bom;
		utf8_bom.push_back(-17);
		utf8_bom.push_back(-69);
		utf8_bom.push_back(-65);
		file_stream << utf8_bom;
	}

	file_stream << ini_str;
	return true;
}

void CIniHelper::LoadFontData(const wchar_t * AppName, FontInfo & font, const FontInfo& default_font) const
{
	font.name = GetString(AppName, L"font_name", default_font.name).c_str();
	font.size = GetInt(AppName, L"font_size", default_font.size);
	bool style[4];
	GetBoolArray(AppName, L"font_style", style, 4);
	font.bold = style[0];
	font.italic = style[1];
	font.underline = style[2];
	font.strike_out = style[3];
}

void CIniHelper::_WriteString(const wchar_t * AppName, const wchar_t * KeyName, const wstring & str)
{
	wstring app_str{ L"[" };
	app_str.append(AppName).append(L"]");
	size_t app_pos{}, app_end_pos, key_pos;
	app_pos = m_ini_str.find(app_str);
	if (app_pos == wstring::npos)		//找不到AppName，则在最后面添加
	{
		if (!m_ini_str.empty() && m_ini_str.back() != L'\n')
			m_ini_str += L"\n";
		app_pos = m_ini_str.size();
		m_ini_str += app_str;
		m_ini_str += L"\n";
	}
	app_end_pos = m_ini_str.find(L"\n[", app_pos + 2);
	if (app_end_pos != wstring::npos)
		app_end_pos++;

	key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L' ', app_pos);		//查找“\nkey_name ”
	if (key_pos >= app_end_pos)		//如果找不到“\nkey_name ”，则查找“\nkey_name=”
		key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L'=', app_pos);
	if (key_pos >= app_end_pos)				//找不到KeyName，则插入一个
	{
		wchar_t buff[256];
		swprintf_s(buff, L"%s = %s\n", KeyName, str.c_str());
		if (app_end_pos == wstring::npos)
			m_ini_str += buff;
		else
			m_ini_str.insert(app_end_pos, buff);
	}
	else	//找到了KeyName，将等号到换行符之间的文本替换
	{
		size_t str_pos;
		str_pos = m_ini_str.find(L'=', key_pos + 2);
		size_t line_end_pos = m_ini_str.find(L'\n', key_pos + 2);
		if (str_pos > line_end_pos)	//所在行没有等号，则插入一个等号
		{
			m_ini_str.insert(key_pos + wcslen(KeyName) + 1, L" =");
			str_pos = key_pos + wcslen(KeyName) + 2;
		}
		else
		{
			str_pos++;
		}
		size_t str_end_pos;
		str_end_pos = m_ini_str.find(L"\n", str_pos);
		m_ini_str.replace(str_pos, str_end_pos - str_pos, L" " + str);
	}
}

wstring CIniHelper::_GetString(const wchar_t * AppName, const wchar_t * KeyName, const wchar_t* default_str) const
{
	wstring app_str{ L"[" };
	app_str.append(AppName).append(L"]");
	size_t app_pos{}, app_end_pos, key_pos;
	app_pos = m_ini_str.find(app_str);
	if (app_pos == wstring::npos)		//找不到AppName，返回默认字符串
		return default_str;

	app_end_pos = m_ini_str.find(L"\n[", app_pos + 2);
	if (app_end_pos != wstring::npos)
		app_end_pos++;

	key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L' ', app_pos);		//查找“\nkey_name ”
	if (key_pos >= app_end_pos)		//如果找不到“\nkey_name ”，则查找“\nkey_name=”
		key_pos = m_ini_str.find(wstring(L"\n") + KeyName + L'=', app_pos);
	if (key_pos >= app_end_pos)				//找不到KeyName，返回默认字符串
	{
		return default_str;
	}
	else	//找到了KeyName，获取等号到换行符之间的文本
	{
		size_t str_pos;
		str_pos = m_ini_str.find(L'=', key_pos + 2);
		size_t line_end_pos = m_ini_str.find(L'\n', key_pos + 2);
		if (str_pos > line_end_pos)	//所在行没有等号，返回默认字符串
		{
			return default_str;
		}
		else
		{
			str_pos++;
		}
		size_t str_end_pos;
		str_end_pos = m_ini_str.find(L"\n", str_pos);
		//获取文本
		wstring return_str{ m_ini_str.substr(str_pos, str_end_pos - str_pos) };
		//如果前后有空格，则将其删除
		CCommon::StringNormalize(return_str);
		return return_str;
	}
}
