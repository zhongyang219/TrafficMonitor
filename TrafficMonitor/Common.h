#pragma once
#include "CommonData.h"

class CCommon
{
public:
	CCommon();
	~CCommon();
	//将const char*字符串转换成宽字符字符串
	static wstring StrToUnicode(const char* str, bool utf8 = false);

	static string UnicodeToStr(const wchar_t* wstr);

	/*根据数据的大小转换成以KB、MB、GB为单位的字符串
	size：数据的大小，单位为字节
	short_mode：是否使用精简模式（减小小数点位数，单位不显示“B”）
	speed_unit：数据的单位，可以是自动、KB或MB
	hide_unit：是否隐藏单位
	返回值：转换后的字符串
	*/
	static CString DataSizeToString(unsigned int size, bool short_mode = false, SpeedUnit unit = SpeedUnit::AUTO, bool hide_unit = false);

	static CString KBytesToString(unsigned int kb_size);

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

	//计算两个SYSTEMTIME结构时间的差（a-b，只保留时、分、秒）
	static SYSTEMTIME CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b);

	//获取当前程序的路径
	static wstring GetExePath();

	//获取system32文件夹的路径
	static wstring GetSystemPath();

	//在指定位置绘制文本
	static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

	//设置绘图的剪辑区域
	static void SetDrawArea(CDC* pDC, CRect rect);

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

	//获取Windows版本
	static void GetWindowsVersion(int& major_version, int& minor_version, int& build_number);

	//判断当前Windows版本是否为Win10秋季创意者更新或更新的版本
	static bool IsWindows10FallCreatorOrLater();

	//获取URL的内容
	static bool GetURL(const wstring& url, wstring& result);

	//获取外网IP
	static wstring GetInternetIp();

	static void SetRect(CRect& rect, int x, int y, int width, int height);

	//从资源文件载入字符串。其中，front_str、back_str为载入字符串时需要在前面或后面添加的字符串
	static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
	static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);

	static CString IntToString(int n);

	//删除字体名称后面的Bold、Light等字符串，并根据这些字符串设置字体粗细
	static void NormalizeFont(LOGFONT& font);

	//安全的字符串复制函数
	static void WStringCopy(wchar_t* str_dest, int dest_size, const wchar_t* str_source, int source_size = INT_MAX);
};

