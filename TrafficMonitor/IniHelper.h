#pragma once
#define DEF_CH L'\"'		//写入和读取ini文件字符串时，在字符串前后添加的字符

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

protected:
	wstring m_path;
};

