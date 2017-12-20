#pragma once

//储存某一天的历史流量
struct HistoryTraffic
{
	int year;
	int month;
	int day;
	unsigned kBytes;	//当天使用的流量（以KB为单位）

	//比较两个HistoryTraffic对象的日期，如果a的时间大于b，则返回true
	static bool DateGreater(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		if (a.year != b.year)
			return a.year > b.year;
		else if (a.month != b.month)
			return a.month > b.month;
		else if (a.day != b.day)
			return a.day > b.day;
		else
			return false;
	}

	//判断两个HistoryTraffic对象的日期是否相等
	static bool DateEqual(const HistoryTraffic& a, const HistoryTraffic& b)
	{
		return a.year == b.year && a.month == b.month && a.day == b.day;
	}
};

class CCommon
{
public:
	CCommon();
	~CCommon();
	//将const char*字符串转换成宽字符字符串
	static wstring StrToUnicode(const char* str);

	static string UnicodeToStr(const wchar_t* wstr);

	//将一个int类型数据写入ini文件，如果成功则返回true
	static bool WritePrivateProfileIntW(const wchar_t * AppName, const wchar_t * KeyName, int value, const wchar_t * Path);

	//根据数据在大小转换成以KB、MB、GB为单位的字符串
	static CString DataSizeToString(unsigned int size);

	static CString KBytesToString(unsigned int kb_size);

	//返回两个FILETIME结构的时间差
	static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);

	//将一个日志信息str_text写入到file_path文件中
	static void WriteLog(const char* str_text, LPCTSTR file_path);

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

	static wstring GetExePath();

	//在指定位置绘制文本
	static void DrawWindowText(CDC* pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color);

	//为一个Static控件填充指定的颜色
	static void FillStaticColor(CStatic& static_ctr, COLORREF color);

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

	//判断现在是否刚开机，在刚开机3分钟内返回true，否则返回false
	static bool WhenStart();

	//显示鼠标提示
	static CString GetMouseTipsInfo(__int64 today_traffic, int cpu_usage, int memory_usage, int used_memory, int total_memory, bool show_cpu_memory);

};

