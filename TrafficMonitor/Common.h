#pragma once
#include "CommonData.h"

class CCommon
{
public:
	CCommon();
	~CCommon();
	//将const char*字符串转换成宽字符字符串
	static wstring StrToUnicode(const char* str, bool utf8 = false);

	static string UnicodeToStr(const wchar_t* wstr, bool utf8 = false);

	static void StringNormalize(wstring& str);

	/*根据数据的大小转换成以KB、MB、GB为单位的字符串
	size：数据的字节数
	返回值：转换后的字符串
	*/
	static CString DataSizeToString(unsigned int size, const PublicSettingData& cfg);
	static CString DataSizeToString(unsigned int size);

	static CString KBytesToString(unsigned int kb_size);
	static CString KBytesToStringL(__int64 kb_size);

	//返回两个FILETIME结构的时间差
	static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

	//将一个日志信息str_text写入到file_path文件中
	static void WriteLog(const char* str_text, LPCTSTR file_path);
	static void WriteLog(const wchar_t* str_text, LPCTSTR file_path);

	/*
	函数功能：对指定文件在指定的目录下创建其快捷方式
	函数参数：
	lpszLnkFileDir  指定目录，不能为NULL。
	lpszFileName    指定文件，为NULL表示当前进程的EXE文件。
	lpszLnkFileName 快捷方式名称，为NULL表示EXE文件名。
	wHotkey         为0表示不设置快捷键
	pszDescription  备注
	iShowCmd        运行方式，默认为常规窗口
	*/
	static BOOL CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName = NULL, LPCTSTR lpszLnkFileName = NULL, LPCTSTR lpszWorkDir = NULL, WORD wHotkey = 0, LPCTSTR lpszDescription = NULL, int iShowCmd = SW_SHOWNORMAL);

	//获取开始菜单“所有程序”中的“启动”目录的路径
	static wstring GetStartUpPath();

	//获取path路径下的文件或文件夹，并将文件或文件夹名称保存在files容器中。
	static void GetFiles(const wchar_t* path, vector<wstring>& files);

	//判断一个文件是否存在
	static bool FileExist(LPCTSTR file_name);

	static bool MoveAFile(LPCTSTR exist_file, LPCTSTR new_file);

	//计算两个SYSTEMTIME结构时间的差（a-b，只保留时、分、秒）
	static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

	//获取当前程序的目录
	static wstring GetModuleDir();

	//获取system32文件夹的路径
	static wstring GetSystemDir();

	//获取临时文件夹的路径
	static wstring GetTemplateDir();

	//获取Appdata/Local/TrafficMonitor的目录，如果不存在，则会自动创建
	static wstring GetAppDataConfigDir();

	//在指定位置绘制文本
	static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

	////设置绘图的剪辑区域
	//static void SetDrawArea(CDC* pDC, CRect rect);

	/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* 函数名称：IsForegroundFullscreen
	* 功能说明：判断当前正在与用户交互的前台窗口是否是全屏的。
	* 参数说明：无
	* 返回说明：true：是。
	false：否。
	* 线程安全：是
	* 调用样例：IsForegroundFullscreen ()，表示判断当前正在与用户交互的前台窗口是否是全屏的。
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	static bool IsForegroundFullscreen();

	//将一个字符串保存到剪贴板
	static bool CopyStringToClipboard(const wstring& str);

	//获取URL的内容
	static bool GetURL(const wstring& url, wstring& result, bool utf8 = false);

	//获取外网IP地址和IP归属地
	static void GetInternetIp(wstring& ip_address, wstring& ip_location, bool global);

	static void SetRect(CRect& rect, int x, int y, int width, int height);

	//从资源文件载入字符串。其中，front_str、back_str为载入字符串时需要在前面或后面添加的字符串
	static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
	static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);

	//将int类型转换成字符串
	//n：要转换的数值
	//thousand_separation：是否要每隔3位数使用逗号分隔
	//is_unsigned：数值是否是无符号的
	static CString IntToString(int n, bool thousand_separation = false, bool is_unsigned = false);

	//删除字体名称后面的Bold、Light等字符串，并根据这些字符串设置字体粗细
	static void NormalizeFont(LOGFONT& font);

	//安全的字符串复制函数
	static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);

	/// <summary>
	/// 字符串相似度算法-编辑距离法
	/// </summary>
	/// <returns>返回的值为0~1，越大相似度越高</returns>
	static double StringSimilarDegree_LD(const string& srcString, const string& matchString);

	//设置线程语言
	static void SetThreadLanguage(Language language);
};

