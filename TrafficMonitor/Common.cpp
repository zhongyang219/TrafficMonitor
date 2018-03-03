#include "stdafx.h"
#include "Common.h"


CCommon::CCommon()
{
}


CCommon::~CCommon()
{
}

wstring CCommon::StrToUnicode(const char* str, bool utf8)
{
	wstring result;
	int size;
	size = MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, NULL, 0);
	if (size <= 0) return wstring();
	wchar_t* str_unicode = new wchar_t[size + 1];
	MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, str_unicode, size);
	result.assign(str_unicode);
	delete[] str_unicode;
	return result;
}

string CCommon::UnicodeToStr(const wchar_t * wstr)
{
	string result;
	int size{ 0 };
	size = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	if (size <= 0) return string();
	char* str = new char[size + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, size, NULL, NULL);
	result.assign(str);
	delete[] str;
	return result;
}

bool CCommon::WritePrivateProfileIntW(const wchar_t * AppName, const wchar_t * KeyName, int value, const wchar_t * Path)
{
	wchar_t buff[11];
	_itow_s(value, buff, 10);
	return (WritePrivateProfileStringW(AppName, KeyName, buff, Path) != FALSE);
}

CString CCommon::DataSizeToString(unsigned int size, bool short_mode)
{
	CString str;
	if (short_mode)
	{
		//if (size <= 102)			//小于0.1KB时，显示0K
		//	str = _T("0K");
		/*else */if (size < 1024 * 10)					//10KB以下以KB为单位，保留1位小数
			str.Format(_T("%.1fK"), size / 1024.0f);
		else if (size < 1024 * 1024)			//1MB以下以KB为单位，保留整数
			str.Format(_T("%.0fK"), size / 1024.0f);
		else if (size < 1024 * 1024 * 1024)		//1GB以下以MB为单位，保留1位小数
			str.Format(_T("%.1fM"), size / 1024.0f / 1024.0f);
		else
			str.Format(_T("%.2fG"), size / 1024.0f / 1024.0f / 1024.0f);
	}
	else
	{
		if (size < 1024 * 10)					//10KB以下以KB为单位，保留2位小数
			str.Format(_T("%.2fKB"), size / 1024.0f);
		else if (size < 1024 * 1024)			//1MB以下以KB为单位，保留1位小数
			str.Format(_T("%.1fKB"), size / 1024.0f);
		else if (size < 1024 * 1024 * 1024)		//1GB以下以MB为单位，保留2位小数
			str.Format(_T("%.2fMB"), size / 1024.0f / 1024.0f);
		else
			str.Format(_T("%.2fGB"), size / 1024.0f / 1024.0f / 1024.0f);
	}
	return str;
}

CString CCommon::KBytesToString(unsigned int kb_size)
{
	CString k_bytes_str;
	if (kb_size < 1024)
		k_bytes_str.Format(_T("%dKB"), kb_size);
	else if (kb_size < 1024 * 1024)
		k_bytes_str.Format(_T("%.2fMB"), kb_size / 1024.0);
	else
		k_bytes_str.Format(_T("%.2fGB"), kb_size / 1024.0 / 1024.0);
	return k_bytes_str;
}

__int64 CCommon::CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
	return b - a;
}

void CCommon::WriteLog(const char* str_text, LPCTSTR file_path)
{
	SYSTEMTIME cur_time;
	GetLocalTime(&cur_time);
	char buff[32];
	sprintf_s(buff, "%d/%.2d/%.2d %.2d:%.2d:%.2d.%.3d: ", cur_time.wYear, cur_time.wMonth, cur_time.wDay,
		cur_time.wHour, cur_time.wMinute, cur_time.wSecond, cur_time.wMilliseconds);
	ofstream file{ file_path, std::ios::app };	//以追加的方式打开日志文件
	file << buff;
	file << str_text << std::endl;
}

BOOL CCommon::CreateFileShortcut(LPCTSTR lpszLnkFileDir, LPCTSTR lpszFileName, LPCTSTR lpszLnkFileName, LPCTSTR lpszWorkDir, WORD wHotkey, LPCTSTR lpszDescription, int iShowCmd)
{
	if (lpszLnkFileDir == NULL)
		return FALSE;

	HRESULT hr;
	IShellLink     *pLink;  //IShellLink对象指针
	IPersistFile   *ppf; //IPersisFil对象指针

						 //创建IShellLink对象
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pLink);
	if (FAILED(hr))
		return FALSE;

	//从IShellLink对象中获取IPersistFile接口
	hr = pLink->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hr))
	{
		pLink->Release();
		return FALSE;
	}

	TCHAR file_path[MAX_PATH];
	GetModuleFileName(NULL, file_path, MAX_PATH);

	//目标
	if (lpszFileName == NULL)
		pLink->SetPath(file_path);
	else
		pLink->SetPath(lpszFileName);

	//工作目录
	if (lpszWorkDir != NULL)
	{
		pLink->SetWorkingDirectory(lpszWorkDir);
	}
	else
	{
		//设置工作目录为快捷方式目标所在位置
		TCHAR workDirBuf[MAX_PATH];
		if (lpszFileName == NULL)
			wcscpy_s(workDirBuf, file_path);
		else
			wcscpy_s(workDirBuf, lpszFileName);
		LPTSTR pstr = wcsrchr(workDirBuf, _T('\\'));
		*pstr = _T('\0');
		pLink->SetWorkingDirectory(workDirBuf);
	}

	//快捷键
	if (wHotkey != 0)
		pLink->SetHotkey(wHotkey);

	//备注
	if (lpszDescription != NULL)
		pLink->SetDescription(lpszDescription);

	//显示方式
	pLink->SetShowCmd(iShowCmd);


	//快捷方式的路径 + 名称
	wchar_t szBuffer[MAX_PATH];
	if (lpszLnkFileName != NULL) //指定了快捷方式的名称
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, lpszLnkFileName);
	else
	{
		//没有指定名称，就从取指定文件的文件名作为快捷方式名称。
		const wchar_t *pstr;
		if (lpszFileName != NULL)
			pstr = wcsrchr(lpszFileName, L'\\');
		else
			pstr = wcsrchr(file_path, L'\\');

		if (pstr == NULL)
		{
			ppf->Release();
			pLink->Release();
			return FALSE;
		}
		//注意后缀名要从.exe改为.lnk
		swprintf_s(szBuffer, L"%s\\%s", lpszLnkFileDir, pstr);
		int nLen = wcslen(szBuffer);
		szBuffer[nLen - 3] = L'l';
		szBuffer[nLen - 2] = L'n';
		szBuffer[nLen - 1] = L'k';
	}
	//保存快捷方式到指定目录下
	//WCHAR  wsz[MAX_PATH];  //定义Unicode字符串
	//MultiByteToWideChar(CP_ACP, 0, szBuffer, -1, wsz, MAX_PATH);

	hr = ppf->Save(szBuffer, TRUE);

	ppf->Release();
	pLink->Release();
	return SUCCEEDED(hr);
}

wstring CCommon::GetStartUpPath()
{
	LPITEMIDLIST ppidl;
	TCHAR pszStartUpPath[MAX_PATH];
	if (SHGetSpecialFolderLocation(NULL, CSIDL_STARTUP, &ppidl) == S_OK)
	{
		SHGetPathFromIDList(ppidl, pszStartUpPath);
		CoTaskMemFree(ppidl);
	}
	return wstring(pszStartUpPath);
}

void CCommon::GetFiles(const wchar_t* path, vector<wstring>& files)
{
	//文件句柄 
	int hFile = 0;
	//文件信息（用Unicode保存使用_wfinddata_t，多字节字符集使用_finddata_t）
	_wfinddata_t fileinfo;
	wstring file_name;
	if ((hFile = _wfindfirst(wstring(path).append(L"\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			file_name.assign(fileinfo.name);
			if (file_name != L"." && file_name != L"..")
				//files.push_back(wstring(path) + L"\\" + file_name);  //将文件名保存(忽略"."和"..")
				files.push_back(L"\\" + file_name);  //将文件名保存(忽略"."和"..")
		} while (_wfindnext(hFile, &fileinfo) == 0);
	}
	_findclose(hFile);
}

bool CCommon::FileExist(LPCTSTR file_name)
{
	_wfinddata_t fileinfo;
	return (_wfindfirst(file_name, &fileinfo) != -1);
}

SYSTEMTIME CCommon::CompareSystemTime(SYSTEMTIME a, SYSTEMTIME b)
{
	SYSTEMTIME result{};
	short hour = a.wHour - b.wHour;
	short minute = a.wMinute - b.wMinute;
	short second = a.wSecond - b.wSecond;

	if (second < 0)
	{
		second += 60;
		minute--;
	}

	if (minute < 0)
	{
		minute += 60;
		hour--;
	}

	if (hour < 0)
	{
		hour += 24;
	}
	result.wHour = hour;
	result.wMinute = minute;
	result.wSecond = second;
	return result;
}

wstring CCommon::GetExePath()
{
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	size_t index;
	wstring current_path{ path };
	index = current_path.find_last_of(L'\\');
	current_path = current_path.substr(0, index + 1);
	return current_path;
}

void CCommon::DrawWindowText(CDC * pDC, CRect rect, LPCTSTR lpszString, COLORREF color, COLORREF back_color)
{
	pDC->SetTextColor(color);
	//m_pDC->SetBkMode(TRANSPARENT);
	//用背景色填充矩形区域
	pDC->FillSolidRect(rect, back_color);
	pDC->DrawText(lpszString, rect, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

}

//void CCommon::FillStaticColor(CStatic & static_ctr, COLORREF color)
//{
//	CDC* pDC = static_ctr.GetDC();
//	CRect rect;
//	static_ctr.GetClientRect(&rect);
//	pDC->FillSolidRect(rect, color);
//}

void CCommon::SetDrawArea(CDC * pDC, CRect rect)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	pDC->SelectClipRgn(&rgn);
}


bool CCommon::IsForegroundFullscreen()
{
	bool bFullscreen{ false };		//用于指示前台窗口是否是全屏
	HWND hWnd;
	RECT rcApp;
	RECT rcDesk;
	hWnd = GetForegroundWindow();	//获取当前正在与用户交互的前台窗口句柄
	TCHAR buff[256];
	GetClassName(hWnd, buff, 256);		//获取前台窗口的类名
	CString class_name{ buff };
	if (hWnd != GetDesktopWindow() && class_name!=_T("WorkerW") && hWnd != GetShellWindow())//如果前台窗口不是桌面窗口，也不是控制台窗口
	{
		GetWindowRect(hWnd, &rcApp);	//获取前台窗口的坐标
		GetWindowRect(GetDesktopWindow(), &rcDesk);	//根据桌面窗口句柄，获取整个屏幕的坐标
		if (rcApp.left <= rcDesk.left && //如果前台窗口的坐标完全覆盖住桌面窗口，就表示前台窗口是全屏的
			rcApp.top <= rcDesk.top &&
			rcApp.right >= rcDesk.right &&
			rcApp.bottom >= rcDesk.bottom)
		{
			bFullscreen = true;
		}
	}//如果前台窗口是桌面窗口，或者是控制台窗口，就直接返回不是全屏
	return bFullscreen;
}

bool CCommon::CopyStringToClipboard(const wstring & str)
{
	if (OpenClipboard(NULL))
	{
		HGLOBAL clipbuffer;
		EmptyClipboard();
		size_t size = (str.size() + 1) * 2;
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, size);
		memcpy_s(GlobalLock(clipbuffer), size, str.c_str(), size);
		GlobalUnlock(clipbuffer);
		if (SetClipboardData(CF_UNICODETEXT, clipbuffer) == NULL)
			return false;
		CloseClipboard();
		return true;
	}
	else return false;
}

bool CCommon::WhenStart(int time, bool write_log)
{
	int tick_count = GetTickCount();
	if (write_log)
	{
		char buff[128];
		sprintf_s(buff, "start time is %dms, no_multistart_warning_time is %d", tick_count, time);
		WriteLog(buff, _T(".\\start.log"));
	}
	return (tick_count < time);
}

CString CCommon::GetMouseTipsInfo(__int64 today_traffic, int cpu_usage, int memory_usage, int used_memory, int total_memory, bool show_cpu_memory)
{
	CString tip_info;
	if (show_cpu_memory)
	{
		tip_info.Format(_T("今日已使用流量：%s\r\n内存使用：%s/%s"),
			CCommon::KBytesToString(static_cast<unsigned int>(today_traffic / 1024)),
			CCommon::KBytesToString(used_memory), CCommon::KBytesToString(total_memory));
	}
	else
	{
		tip_info.Format(_T("今日已使用流量：%s\r\nCPU使用：%d%%\r\n内存使用：%s/%s (%d%%)"),
			CCommon::KBytesToString(static_cast<unsigned int>(today_traffic / 1024)),
			cpu_usage,
			CCommon::KBytesToString(used_memory), CCommon::KBytesToString(total_memory),
			memory_usage);
	}
	return tip_info;
}

void CCommon::GetWindowsVersion(int & major_version, int & minor_version, int & build_number)
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
	major_version = dwMajorVer;
	minor_version = dwMinorVer;
	build_number = dwBuildNumber;
}

bool CCommon::IsWindows10FallCreatorOrLater()
{
	int major_version, minor_version, build_number;
	GetWindowsVersion(major_version, minor_version, build_number);
	if (major_version > 10)
		return true;
	else if (major_version == 10 && minor_version > 0)
		return true;
	else if (major_version == 10 && minor_version == 0 && build_number >= 16299)
		return true;
	else return false;
}

bool CCommon::GetURL(const wstring & url, wstring & result)
{
	bool sucessed{ false };
	CInternetSession session{};
	CHttpFile* pfile{};
	try
	{
		pfile = (CHttpFile *)session.OpenURL(url.c_str());
		DWORD dwStatusCode;
		pfile->QueryInfoStatusCode(dwStatusCode);
		if (dwStatusCode == HTTP_STATUS_OK)
		{
			CString content;
			CString data;
			while (pfile->ReadString(data))
			{
				content += data;
			}
			result = StrToUnicode((const char*)content.GetString());
			sucessed = true;
		}
		pfile->Close();
		delete pfile;
		session.Close();
	}
	catch (CInternetException* e)
	{
		if (pfile != nullptr)
		{
			pfile->Close();
			delete pfile;
		}
		session.Close();
		sucessed = false;
	}
	return sucessed;
}
