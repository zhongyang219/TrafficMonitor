//ini读写类
//使用时将ini文件路径通过构造函数参数传递
//在向ini文件写入数据时，需要在最后调用Save()函数以将更改保存到文件
//默认以UTF8_BOM编码保存，如果要以ANSI保存，请调用SetSaveAsUTF8(false);
#pragma once
#include "CommonData.h"
#include "Common.h"

class CIniHelper
{
public:
	CIniHelper(const wstring& file_path);
	~CIniHelper();

	void SetSaveAsUTF8(bool utf8);

	void WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str);
	wstring GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str) const;
	void WriteInt(const wchar_t * AppName, const wchar_t * KeyName, int value);
	int GetInt(const wchar_t * AppName, const wchar_t * KeyName, int default_value) const;
	void WriteBool(const wchar_t * AppName, const wchar_t * KeyName, bool value);
	bool GetBool(const wchar_t * AppName, const wchar_t * KeyName, bool default_value) const;
	void WriteIntArray(const wchar_t * AppName, const wchar_t * KeyName, const int* values, int size);		//写入一个int数组，元素个数为size
	void GetIntArray(const wchar_t * AppName, const wchar_t * KeyName, int* values, int size, int default_value = 0) const;		//读取一个int数组，储存到values，元素个数为size
	void WriteBoolArray(const wchar_t * AppName, const wchar_t * KeyName, const bool* values, int size);
	void GetBoolArray(const wchar_t * AppName, const wchar_t * KeyName, bool* values, int size, bool default_value = false) const;
    void WriteStringList(const wchar_t* AppName, const wchar_t* KeyName, const vector<wstring>& values);      //写入一个字符串列表，由于保存到ini文件中时字符串前后会加上引号，所以字符串中不能包含引号
    void GetStringList(const wchar_t* AppName, const wchar_t* KeyName, vector<wstring>& values, const vector<wstring>& default_value) const;

	void SaveFontData(const wchar_t * AppName, const FontInfo& font);
	void LoadFontData(const wchar_t * AppName, FontInfo& font, const FontInfo& default_font) const;

    void LoadMainWndColors(const wchar_t * AppName, const wchar_t * KeyName, std::map<CommonDisplayItem, COLORREF>& text_colors, COLORREF default_color);
    void SaveMainWndColors(const wchar_t * AppName, const wchar_t * KeyName, const std::map<CommonDisplayItem, COLORREF>& text_colors);

    void LoadTaskbarWndColors(const wchar_t * AppName, const wchar_t * KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, const wchar_t* default_str);
    void LoadTaskbarWndColors(const wchar_t * AppName, const wchar_t * KeyName, std::map<CommonDisplayItem, TaskbarItemColor>& text_colors, COLORREF default_color);
    void SaveTaskbarWndColors(const wchar_t * AppName, const wchar_t * KeyName, const std::map<CommonDisplayItem, TaskbarItemColor>& text_colors);

    void LoadPluginDisplayStr(bool is_main_window);
    void SavePluginDisplayStr(bool is_main_window);

	bool Save();		//将ini文件保存到文件，成功返回true

protected:
	wstring m_file_path;
	wstring m_ini_str;
	bool m_save_as_utf8{ true };		//是否以及UTF8编码保存

	void _WriteString(const wchar_t* AppName, const wchar_t* KeyName, const wstring& str);
	wstring _GetString(const wchar_t* AppName, const wchar_t* KeyName, const wchar_t* default_str) const;

    static wstring MergeStringList(const vector<wstring>& values);
    static void SplitStringList(vector<wstring>& values, wstring str_value);
};
