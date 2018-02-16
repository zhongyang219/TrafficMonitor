
// TrafficMonitor.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrafficMonitorApp

BEGIN_MESSAGE_MAP(CTrafficMonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CTrafficMonitorApp 构造

CTrafficMonitorApp::CTrafficMonitorApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

void CTrafficMonitorApp::LoadConfig()
{
	//m_no_multistart_warning_time = GetPrivateProfileInt(_T("other"), _T("no_multistart_warning_time"), 300000, m_config_path.c_str());
	m_no_multistart_warning = (GetPrivateProfileInt(_T("other"), _T("no_multistart_warning"), 0, m_config_path.c_str()) != 0);
	m_general_data.check_update_when_start = (GetPrivateProfileInt(_T("general"), _T("check_update_when_start"), 1, m_config_path.c_str()) != 0);
}

void CTrafficMonitorApp::SaveConfig()
{
	//CCommon::WritePrivateProfileIntW(_T("other"), _T("no_multistart_warning_time"), m_no_multistart_warning_time, m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(_T("other"), _T("no_multistart_warning"), m_no_multistart_warning, m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(_T("general"), _T("check_update_when_start"), m_general_data.check_update_when_start, m_config_path.c_str());
}

int CTrafficMonitorApp::DPI(int pixel)
{
	return m_dpi * pixel / 96;
}

void CTrafficMonitorApp::GetDPI(CWnd* pWnd)
{
	CWindowDC dc(pWnd);
	HDC hDC = dc.GetSafeHdc();
	m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
}

void CTrafficMonitorApp::CheckUpdate(bool message)
{
	CWaitCursor wait_cursor;
	wstring version_info;
	if (!CCommon::GetURL(L"https://raw.githubusercontent.com/zhongyang219/TrafficMonitor/master/version.info", version_info))		//获取版本信息
	{
		if(message)
			AfxMessageBox(_T("检查更新失败，请检查你的网络连接！"), MB_OK | MB_ICONWARNING);
		return;
	}

	size_t index, index1, index2, index3, index4, index5;
	index = version_info.find(L"<version>");
	index1 = version_info.find(L"</version>");
	index2 = version_info.find(L"<link>");
	index3 = version_info.find(L"</link>");
	index4 = version_info.find(L"<contents>");
	index5 = version_info.find(L"</contents>");
	wstring version;		//程序版本
	wstring link;			//下载链接
	wstring contents;		//更新内容
	version = version_info.substr(index + 9, index1 - index - 9);
	link = version_info.substr(index2 + 6, index3 - index2 - 6);
	contents = version_info.substr(index4 + 10, index5 - index4 - 10);
	CString contents_str = contents.c_str();
	contents_str.Replace(L"\\n", L"\r\n");
	if (index == wstring::npos || index1 == wstring::npos || index2 == wstring::npos || index3 == wstring::npos || version.empty() || link.empty())
	{
		if (message)
			AfxMessageBox(_T("检查更新失败，从远程更新文件获取到了错误的信息，请联系作者！"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (version > VERSION)		//如果服务器上的版本大于本地版本
	{
		CString info;
		if (contents.empty())
			info.Format(_T("检测到新版本 V%s，是否前往更新？"), version.c_str());
		else
			info.Format(_T("检测到新版本 V%s，更新内容：\r\n%s\r\n是否前往更新？"), version.c_str(), contents_str);
			
		if (AfxMessageBox(info, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			ShellExecute(NULL, _T("open"), link.c_str(), NULL, NULL, SW_SHOW);		//转到下载链接
		}
	}
	else
	{
		if(message)
			AfxMessageBox(_T("当前已经是最新版本。"), MB_OK | MB_ICONINFORMATION);
	}
}

UINT CTrafficMonitorApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
	CheckUpdate(false);
	return 0;
}

void CTrafficMonitorApp::SetAutoRun(bool auto_run)
{
	CRegKey key;
	wstring module_path;
	if (m_module_path.find(L' ') != wstring::npos)
	{
		//如果路径中有空格，则需要在程序路径前后添加双引号
		module_path = L'\"';
		module_path += m_module_path;
		module_path += L'\"';
	}
	else
	{
		module_path = m_module_path;
	}
	//打开注册表项
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
	{
		AfxMessageBox(_T("无法实现开机自启动，在注册表中找不到相应的键值！"), MB_OK | MB_ICONWARNING);
		return;
	}
	if (auto_run)		//写入注册表项
	{
		if (key.SetStringValue(_T("TrafficMonitor"), module_path.c_str()) != ERROR_SUCCESS)
		{
			AfxMessageBox(_T("注册表项写入失败，可能该键值没有权限访问！"), MB_OK | MB_ICONWARNING);
			return;
		}
	}
	else		//删除注册表项
	{
		//删除前先检查注册表项是否存在，如果不存在，则直接返回
		wchar_t buff[256];
		ULONG size{ 256 };
		if (key.QueryStringValue(_T("TrafficMonitor"), buff, &size) != ERROR_SUCCESS)
			return;
		if (key.DeleteValue(_T("TrafficMonitor")) != ERROR_SUCCESS)
		{
			AfxMessageBox(_T("注册表项删除失败，可能该键值没有权限访问！"), MB_OK | MB_ICONWARNING);
			return;
		}
	}
}

bool CTrafficMonitorApp::GetAutoRun()
{
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
	{
		return false;
	}
	wchar_t buff[256];
	ULONG size{ 256 };
	if (key.QueryStringValue(_T("TrafficMonitor"), buff, &size) != ERROR_SUCCESS)
	{
		return false;
	}
	//只要找到了"TrafficMonitor"这个键，则返回true
	return true;
}


// 唯一的一个 CTrafficMonitorApp 对象

CTrafficMonitorApp theApp;


// CTrafficMonitorApp 初始化

BOOL CTrafficMonitorApp::InitInstance()
{
	//设置配置文件的路径
#ifdef _DEBUG
	m_config_path = L".\\config.ini";
	m_history_traffic_path = L".\\history_traffic.dat";
	m_log_path = L".\\error.log";
	m_skin_path = L".\\skins";
#else
	wstring exe_path = CCommon::GetExePath();
	m_config_path = exe_path + L"config.ini";
	m_history_traffic_path = exe_path + L"history_traffic.dat";
	m_log_path = exe_path + L"error.log";
	m_skin_path = exe_path + L"skins";
#endif

	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	m_module_path = path;

	m_is_windows10_fall_creator = CCommon::IsWindows10FallCreatorOrLater();

	//从ini文件载入设置
	LoadConfig();

#ifndef _DEBUG
	wstring cmd_line{ m_lpCmdLine };
	bool is_restart{ cmd_line.find(L"RestartByRestartManager") != wstring::npos };		//如果命令行参数中含有字符串“RestartByRestartManager”则说明程序是被Windows重新启动的
	//bool when_start{ CCommon::WhenStart(m_no_multistart_warning_time) };
	if (is_restart && m_is_windows10_fall_creator)		//当前Windows版本是秋季创意者更新时，如果程序被重新启动，则直接退出程序
	{
		//AfxMessageBox(_T("调试信息：程序已被Windows的重启管理器重新启动。"));
		return FALSE;
	}

	//检查是否已有实例正在运行
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, _T("FirstName"));
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//char buff[128];
			//string cmd_line_str{ CCommon::UnicodeToStr(cmd_line.c_str()) };
			//sprintf_s(buff, "when_start=%d, m_no_multistart_warning=%d, cmd_line=%s", when_start, m_no_multistart_warning, cmd_line_str.c_str());
			//CCommon::WriteLog(buff, _T(".\\start.log"));
			if (!m_no_multistart_warning)
			{
				AfxMessageBox(_T("已经有一个程序正在运行。"));
			}
			return FALSE;
		}
	}
#endif

	SaveConfig();

	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。  否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	//启动时检查更新
	if (m_general_data.check_update_when_start)
	{
		m_pUpdateThread = AfxBeginThread(CheckUpdateThreadFunc, NULL);
	}

	CTrafficMonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

