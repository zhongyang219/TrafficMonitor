#pragma once
#include "CommonData.h"

class CIniHelper
{
public:
	CIniHelper();
	~CIniHelper();

	void SetPath(const wstring& path);

	bool WriteString(const wchar_t* AppName, const wchar_t* KeyName, wstring str);
	wstring GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str);
	bool WriteInt(const wchar_t * AppName, const wchar_t * KeyName, int value);
	int GetInt(const wchar_t * AppName, const wchar_t * KeyName, int default_value);
	bool WriteBool(const wchar_t * AppName, const wchar_t * KeyName, bool value);
	bool GetBool(const wchar_t * AppName, const wchar_t * KeyName, bool default_value);
	bool WriteIntArray(const wchar_t * AppName, const wchar_t * KeyName, const int* values, int size);		//写入一个int数组，元素个数为size
	bool GetIntArray(const wchar_t * AppName, const wchar_t * KeyName, int* values, int size, int default_value = 0);		//读取一个int数组，储存到values，元素个数为size
	bool WriteBoolArray(const wchar_t * AppName, const wchar_t * KeyName, const bool* values, int size);
	void GetBoolArray(const wchar_t * AppName, const wchar_t * KeyName, bool* values, int size, bool default_value = false);

	void SaveFontData(const wchar_t * AppName, const FontInfo& font);
	void LoadFontData(const wchar_t * AppName, FontInfo& font, const FontInfo& default_font);

protected:
	wstring m_path;
};

