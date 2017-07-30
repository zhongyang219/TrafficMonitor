
// TrafficMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据

protected:
	CStatic m_about_img;		//关于对话框中的图片
	CStaticEx m_mail{ true };	//“联系作者”超链接
	CStaticEx m_check_update{ true };	//“检查更新”超链接
	CStaticEx m_donate{ true };	//“捐助”超链接
	CToolTipCtrl m_Mytip;		//鼠标指向时的工具提示
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnStnClickedStaticDonate();
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_STN_CLICKED(IDC_STATIC_DONATE, &CAboutDlg::OnStnClickedStaticDonate)
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_ABOUT, m_about_img);
	DDX_Control(pDX, IDC_STATIC_MAIL, m_mail);
	DDX_Control(pDX, IDC_STATIC_CHECK_UPDATE, m_check_update);
	DDX_Control(pDX, IDC_STATIC_DONATE, m_donate);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CRect rect;
	GetClientRect(rect);
	rect.bottom = rect.Height() * 2 / 5;	//图片高度占对话框高度的2/5
	m_about_img.MoveWindow(rect);
	m_mail.SetURL(_T("mailto:zhongyang219@hotmail.com"));	//设置超链接
	m_check_update.SetURL(_T("http://pan.baidu.com/s/1c1LkPQ4"));
	
	m_Mytip.Create(this);
	m_Mytip.AddTool(&m_mail, _T("向作者发送电子邮件"));
	m_Mytip.AddTool(&m_check_update, _T("到百度网盘链接查看是否有更新"));
	m_Mytip.AddTool(&m_donate, _T("捐助作者"));
	m_Mytip.SetDelayTime(300);	//设置延迟

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CAboutDlg::OnStnClickedStaticDonate()
{
	CDonateDlg donateDlg;
	donateDlg.DoModal();
}

// CTrafficMonitorDlg 对话框

//静态成员初始化
unsigned int CTrafficMonitorDlg::m_WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };	//注册任务栏建立的消息

CTrafficMonitorDlg::CTrafficMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAFFICMONITOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	LoadConfig();	//启动时从ini文件载入设置
	GetScreenSize();	//获取屏幕大小
}

CTrafficMonitorDlg::~CTrafficMonitorDlg()
{
	free(m_pIfTable);

	if (m_tBarDlg != nullptr)
	{
		delete m_tBarDlg;
		m_tBarDlg = nullptr;
	}
}

void CTrafficMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_UP, m_disp_up);
	DDX_Control(pDX, IDC_STATIC_DOWN, m_disp_down);
	DDX_Control(pDX, IDC_STATIC_CPU, m_disp_cpu);
	DDX_Control(pDX, IDC_STATIC_MEMORY, m_disp_memory);
}

BEGIN_MESSAGE_MAP(CTrafficMonitorDlg, CDialogEx)
//	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
//	ON_WM_NCHITTEST()
	ON_WM_TIMER()
//	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_NETWORK_INFO, &CTrafficMonitorDlg::OnNetworkInfo)
	ON_COMMAND(ID_ALWAYS_ON_TOP, &CTrafficMonitorDlg::OnAlwaysOnTop)
	ON_UPDATE_COMMAND_UI(ID_ALWAYS_ON_TOP, &CTrafficMonitorDlg::OnUpdateAlwaysOnTop)
	ON_WM_INITMENUPOPUP()
	ON_COMMAND(ID_TRANSPARENCY_100, &CTrafficMonitorDlg::OnTransparency100)
	ON_COMMAND(ID_TRANSPARENCY_80, &CTrafficMonitorDlg::OnTransparency80)
	ON_COMMAND(ID_TRANSPARENCY_60, &CTrafficMonitorDlg::OnTransparency60)
	ON_COMMAND(ID_TRANSPARENCY_40, &CTrafficMonitorDlg::OnTransparency40)
//	ON_UPDATE_COMMAND_UI(ID_TRANSPARENCY_100, &CTrafficMonitorDlg::OnUpdateTransparency100)
//	ON_UPDATE_COMMAND_UI(ID_TRANSPARENCY_80, &CTrafficMonitorDlg::OnUpdateTransparency80)
//	ON_UPDATE_COMMAND_UI(ID_TRANSPARENCY_60, &CTrafficMonitorDlg::OnUpdateTransparency60)
//	ON_UPDATE_COMMAND_UI(ID_TRANSPARENCY_40, &CTrafficMonitorDlg::OnUpdateTransparency40)
	ON_WM_CLOSE()
	ON_WM_INITMENU()
//	ON_WM_CREATE()
	ON_COMMAND(ID_LOCK_WINDOW_POS, &CTrafficMonitorDlg::OnLockWindowPos)
	ON_UPDATE_COMMAND_UI(ID_LOCK_WINDOW_POS, &CTrafficMonitorDlg::OnUpdateLockWindowPos)
	ON_WM_MOVE()
//	ON_WM_LBUTTONUP()
	ON_MESSAGE(MY_WM_NOTIFYICON, &CTrafficMonitorDlg::OnNotifyIcon)
	ON_COMMAND(ID_SHOW_NOTIFY_ICON, &CTrafficMonitorDlg::OnShowNotifyIcon)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NOTIFY_ICON, &CTrafficMonitorDlg::OnUpdateShowNotifyIcon)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SHOW_CPU_MEMORY, &CTrafficMonitorDlg::OnShowCpuMemory)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CPU_MEMORY, &CTrafficMonitorDlg::OnUpdateShowCpuMemory)
	ON_COMMAND(ID_MOUSE_PENETRATE, &CTrafficMonitorDlg::OnMousePenetrate)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_PENETRATE, &CTrafficMonitorDlg::OnUpdateMousePenetrate)
	ON_COMMAND(ID_TEXT_COLOR, &CTrafficMonitorDlg::OnTextColor)
	ON_COMMAND(ID_SHOW_TASK_BAR_WND, &CTrafficMonitorDlg::OnShowTaskBarWnd)
	ON_UPDATE_COMMAND_UI(ID_SHOW_TASK_BAR_WND, &CTrafficMonitorDlg::OnUpdateShowTaskBarWnd)
	ON_COMMAND(ID_APP_ABOUT, &CTrafficMonitorDlg::OnAppAbout)
	ON_COMMAND(ID_SHOW_CPU_MEMORY2, &CTrafficMonitorDlg::OnShowCpuMemory2)
	ON_COMMAND(ID_AUTO_RUN_WHEN_START, &CTrafficMonitorDlg::OnAutoRunWhenStart)
	ON_COMMAND(ID_HIDE_MAIN_WND, &CTrafficMonitorDlg::OnHideMainWnd)
	ON_UPDATE_COMMAND_UI(ID_HIDE_MAIN_WND, &CTrafficMonitorDlg::OnUpdateHideMainWnd)
	ON_COMMAND(ID_CHANGE_SKIN, &CTrafficMonitorDlg::OnChangeSkin)
	ON_UPDATE_COMMAND_UI(ID_AUTO_RUN_WHEN_START, &CTrafficMonitorDlg::OnUpdateAutoRunWhenStart)
	ON_REGISTERED_MESSAGE(m_WM_TASKBARCREATED, &CTrafficMonitorDlg::OnTaskBarCreated)
	ON_COMMAND(ID_SET_FONT, &CTrafficMonitorDlg::OnSetFont)
	ON_COMMAND(ID_TRAFFIC_HISTORY, &CTrafficMonitorDlg::OnTrafficHistory)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


void CTrafficMonitorDlg::ShowInfo()
{
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed);

	if (!m_swap_up_down)
	{
		str.Format(_T("上传: %s/s"), out_speed.GetString());
		m_disp_up.SetWindowTextEx(str);
		str.Format(_T("下载: %s/s"), in_speed.GetString());
		m_disp_down.SetWindowTextEx(str);
	}
	else		//交换上传和下载位置
	{
		str.Format(_T("下载: %s/s"), in_speed.GetString());
		m_disp_up.SetWindowTextEx(str);
		str.Format(_T("上传: %s/s"), out_speed.GetString());
		m_disp_down.SetWindowTextEx(str);
	}
	if (m_show_cpu_memory)
	{
		str.Format(_T("CPU: %d%%"), theApp.m_cpu_usage);
		m_disp_cpu.SetWindowTextEx(str);
		str.Format(_T("内存: %d%%"), theApp.m_memory_usage);
		m_disp_memory.SetWindowTextEx(str);
	}
	else
	{
		m_disp_cpu.SetWindowText(_T(""));
		m_disp_memory.SetWindowText(_T(""));
	}
}

void CTrafficMonitorDlg::SetTransparency()
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, m_transparency * 255 / 100, LWA_ALPHA);  //透明度取值范围为0~255
}

void CTrafficMonitorDlg::SetAlwaysOnTop()
{
	if (m_always_on_top)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//设置置顶
	else
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
}

void CTrafficMonitorDlg::SetMousePenetrate()
{
	//CMenu* pContextMenu = m_menu.GetSubMenu(0);
	if (m_mouse_penetrate)
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//设置鼠标穿透
		//if (!m_show_notify_icon && !m_show_task_bar_wnd)	//鼠标穿透且没有显示任务栏窗口时，如果通知图标没有显示，则将它显示出来，否则无法呼出右键菜单
		if (!theApp.m_show_notify_icon)	//鼠标穿透时，如果通知图标没有显示，则将它显示出来，否则无法呼出右键菜单
		{
			//添加通知栏图标
			AddNotifyIcon();
			theApp.m_show_notify_icon = true;
		}
		//pContextMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);	//鼠标穿透时，禁止“显示通知栏图标”菜单项，不允许去掉通知栏图标，否则无法呼出右键菜单
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//取消鼠标穿透
		//if (!m_hide_main_window)		//在取消鼠标穿透时，只有没有选中“隐藏主窗口”时才恢复“显示通知栏图标”菜单项为可用
		//	pContextMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);	//恢复“显示通知栏图标”菜单项
	}
}

void CTrafficMonitorDlg::CheckWindowPos()
{
	CRect rect;
	GetWindowRect(rect);
	if (rect.left < 0)
	{
		rect.MoveToX(0);
		MoveWindow(rect);
	}
	if (rect.top < 0)
	{
		rect.MoveToY(0);
		MoveWindow(rect);
	}
	if (rect.right > m_screen_width)
	{
		rect.MoveToX(m_screen_width - rect.Width());
		MoveWindow(rect);
	}
	if (rect.bottom > m_screen_height)
	{
		rect.MoveToY(m_screen_height - rect.Height());
		MoveWindow(rect);
	}
}

void CTrafficMonitorDlg::GetScreenSize(int compensition_value)
{
	m_screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
	m_screen_height = GetSystemMetrics(SM_CYFULLSCREEN) + compensition_value;
}

void CTrafficMonitorDlg::LoadConfig()
{
	m_transparency = GetPrivateProfileInt(_T("config"), _T("transparency"), 100, theApp.m_config_path.c_str());
	m_always_on_top = (GetPrivateProfileInt(_T("config"), _T("always_on_top"), 0, theApp.m_config_path.c_str()) != 0);
	m_lock_window_pos = (GetPrivateProfileInt(_T("config"), _T("lock_window_pos"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_show_notify_icon = (GetPrivateProfileInt(_T("config"), _T("show_notify_icon"), 1, theApp.m_config_path.c_str()) != 0);
	m_show_cpu_memory = (GetPrivateProfileInt(_T("config"), _T("show_cpu_memory"), 0, theApp.m_config_path.c_str()) != 0);
	m_mouse_penetrate = (GetPrivateProfileInt(_T("config"), _T("mouse_penetrate"), 0, theApp.m_config_path.c_str()) != 0);
	m_show_task_bar_wnd = (GetPrivateProfileInt(_T("config"), _T("show_task_bar_wnd"), 0, theApp.m_config_path.c_str()) != 0);
	m_position_x = GetPrivateProfileInt(_T("config"), _T("position_x"), -1, theApp.m_config_path.c_str());
	m_position_y = GetPrivateProfileInt(_T("config"), _T("position_y"), -1, theApp.m_config_path.c_str());
	m_auto_select = (GetPrivateProfileInt(_T("connection"), _T("auto_select"), 1, theApp.m_config_path.c_str()) != 0);
	//m_connection_selected = GetPrivateProfileInt(_T("connection"), _T("connection_selected"), -1, theApp.m_config_path.c_str());
	m_text_color = GetPrivateProfileInt(_T("config"), _T("text_color"), 0, theApp.m_config_path.c_str());
	theApp.m_hide_main_window = (GetPrivateProfileInt(_T("config"), _T("hide_main_window"), 0, theApp.m_config_path.c_str()) != 0);
	wchar_t buff[256];
	GetPrivateProfileStringW(L"connection", L"connection_name", L"", buff, 256, theApp.m_config_path.c_str());
	m_connection_name = CCommon::UnicodeToStr(buff);
	//m_skin_selected = GetPrivateProfileInt(_T("config"), _T("skin_selected"), 0, theApp.m_config_path.c_str());
	wchar_t wbuff[32];
	GetPrivateProfileString(_T("config"), _T("skin_selected"), _T(""), wbuff, 32, theApp.m_config_path.c_str());
	m_skin_name = wbuff;
	m_swap_up_down = (GetPrivateProfileInt(_T("config"), _T("swap_up_down"), 0, theApp.m_config_path.c_str()) != 0);
	GetPrivateProfileString(_T("config"), _T("font_name"), _T("微软雅黑"), m_font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	m_font_size = GetPrivateProfileInt(_T("config"), _T("font_size"), 10, theApp.m_config_path.c_str());
}

void CTrafficMonitorDlg::SaveConfig()
{
	//保存前先获取窗口的位置
	CRect rect;
	GetWindowRect(rect);
	m_position_x = rect.left;
	m_position_y = rect.top;
	if (!CCommon::WritePrivateProfileIntW(L"config", L"transparency", m_transparency, theApp.m_config_path.c_str()))
	{
		if (m_cannot_save_config_warning)
			MessageBox(_T("警告：无法保存设置，请检查是否有向程序所在目录下写入数据的权限！以管理员身份运行程序可能会解决问题。"), NULL, MB_ICONWARNING);
		m_cannot_save_config_warning = false;
		return;
	}
	CCommon::WritePrivateProfileIntW(L"config", L"always_on_top", m_always_on_top, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"lock_window_pos", m_lock_window_pos, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_notify_icon", theApp.m_show_notify_icon, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_cpu_memory", m_show_cpu_memory, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"mouse_penetrate", m_mouse_penetrate, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_task_bar_wnd", m_show_task_bar_wnd, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"position_x", m_position_x, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"position_y", m_position_y, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"connection", L"auto_select", m_auto_select, theApp.m_config_path.c_str());
	//CCommon::WritePrivateProfileIntW(L"connection", L"connection_selected", m_connection_selected, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"text_color", m_text_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"hide_main_window", theApp.m_hide_main_window, theApp.m_config_path.c_str());
	m_connection_name = m_connections[m_connection_selected].description;
	WritePrivateProfileStringW(L"connection", L"connection_name", CCommon::StrToUnicode(m_connection_name.c_str()).c_str(), theApp.m_config_path.c_str());
	//CCommon::WritePrivateProfileIntW(L"config", L"skin_selected", m_skin_selected, theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("config"), _T("skin_selected"), m_skin_name.c_str(), theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("config"), _T("font_name"), m_font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"font_size", m_font_size, theApp.m_config_path.c_str());
}

void CTrafficMonitorDlg::AutoSelect()
{
	unsigned int max_in_out_bytes{};
	unsigned int in_out_bytes;
	//m_connection_selected = m_connections[0].index;
	m_connection_selected = 0;
	//自动选择连接时，查找已发送和已接收字节数之和最多的那个连接，并将其设置为当前查看的连接
	for (int i{}; i<m_connections.size(); i++)
	{
		if (m_pIfTable->table[m_connections[i].index].dwOperStatus == IF_OPER_STATUS_OPERATIONAL)		//只选择网络状态为正常的连接
		{
			in_out_bytes = m_pIfTable->table[m_connections[i].index].dwInOctets + m_pIfTable->table[m_connections[i].index].dwOutOctets;
			if (in_out_bytes > max_in_out_bytes)
			{ 
				max_in_out_bytes = in_out_bytes;
				m_connection_selected = i;
			}
		}
	}
}

void CTrafficMonitorDlg::IniConnection()
{
	//为m_pIfTable开辟所需大小的内存
	free(m_pIfTable);
	m_dwSize = sizeof(MIB_IFTABLE);
	m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);
	int rtn;
	rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	if (rtn == ERROR_INSUFFICIENT_BUFFER)	//如果函数返回值为ERROR_INSUFFICIENT_BUFFER，说明m_pIfTable的大小不够
	{
		free(m_pIfTable);
		m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);	//用新的大小重新开辟一块内存
	}
	//获取当前所有的连接，并保存到m_connections容器中
	m_connections.clear();
	GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	for (unsigned int i{}; i < m_pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)m_pIfTable->table[i].bDescr;
		if (m_pIfTable->table[i].dwInOctets > 0 || m_pIfTable->table[i].dwOutOctets > 0 || descr == m_connection_name)		//查找接收或发送数据量大于0的连接和上次选择的连接
		{
			m_connections.emplace_back(i, descr, m_pIfTable->table[i].dwInOctets, m_pIfTable->table[i].dwOutOctets);
		}
	}
	if (m_connections.empty())
		m_connections.emplace_back(0, string((const char*)m_pIfTable->table[0].bDescr), 0, 0);

	//if (m_connection_selected < 0 || m_connection_selected >= m_connections.size() || m_auto_select)
	//	AutoSelect();
	//选择网络连接
	if (m_auto_select)	//自动选择
	{
		if (m_restart_cnt != -1)	//当m_restart_cnt不等于-1时，即不是第一次初始化时，需要延时5秒再重新初始化连接
		{
			KillTimer(DELAY_TIMER);
			SetTimer(DELAY_TIMER, 5000, NULL);
		}
		else
		{
			AutoSelect();
		}
	}
	else		//查找网络名为上次选择的连接
	{
		m_connection_selected = 0;
		for (size_t i{}; i < m_connections.size(); i++)
		{
			if (m_connections[i].description == m_connection_name)
				m_connection_selected = i;
		}
	}
	m_connection_name = m_connections[m_connection_selected].description;

	//根据已获取到的连接在菜单中添加相应项目
	m_menu.DestroyMenu();
	m_menu.LoadMenu(IDR_MENU1); //装载右键菜单
	m_select_connection_menu = m_menu.GetSubMenu(0)->GetSubMenu(0);		//设置“选择网卡”子菜单项
	CString connection_descr;
	for (int i{}; i < m_connections.size(); i++)
	{
		connection_descr = CCommon::StrToUnicode(m_connections[i].description.c_str()).c_str();
		m_select_connection_menu->AppendMenu(MF_STRING | MF_ENABLED, ID_SELETE_CONNECTION + i + 1, connection_descr);
	}

	m_restart_cnt++;	//记录初始化次数
	m_connection_change_flag = true;
}

void CTrafficMonitorDlg::CloseTaskBarWnd()
{
	m_tBarDlg->OnCancel();
	delete m_tBarDlg;
	m_tBarDlg = nullptr;
}

void CTrafficMonitorDlg::OpenTaskBarWnd()
{
	//如果没有设置主窗口置顶，则在打开任务栏窗口时先设置主窗口置顶，再取消置顶，以解决任务栏窗口的鼠标提示显示在任务栏后面的问题
	if (!m_always_on_top)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//设置置顶
	m_tBarDlg = new CTaskBarDlg;
	//m_tBarDlg->m_hide_main_window = m_hide_main_window;
	//m_tBarDlg->m_in_speed = m_in_speed;
	//m_tBarDlg->m_out_speed = m_out_speed;
	//m_tBarDlg->m_cpu_usage = m_cpu_usage;
	//m_tBarDlg->m_memory_usage = m_memory_usage;
	//m_tBarDlg->m_used_memory = m_used_memory;
	//m_tBarDlg->m_total_memory = m_total_memory;
	//m_tBarDlg->m_today_traffic = m_today_traffic;
	m_tBarDlg->Create(IDD_TASK_BAR_DIALOG, this);
	m_tBarDlg->ShowWindow(SW_SHOW);
	m_tBarDlg->ShowInfo();
	if (!m_always_on_top)
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//取消置顶
}

void CTrafficMonitorDlg::AddNotifyIcon()
{
	if (m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//添加通知栏图标
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
	if (m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::DeleteNotifyIcon()
{
	if (m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//删除通知栏图标
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
	if (m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::SaveHistoryTraffic()
{
	//SYSTEMTIME current_time;
	//GetLocalTime(&current_time);
	//HistoryTraffic traffic;
	//traffic.year = current_time.wYear;
	//traffic.month = current_time.wMonth;
	//traffic.day = current_time.wDay;
	////如果历史流量列表为空或第1个元素的日期不是今天，则添加今天的流量记录
	//if (m_history_traffics.empty() || !HistoryTraffic::DateEqual(m_history_traffics[0], traffic))
	//{
	//	traffic.kBytes = m_today_traffic / 1024;
	//	m_history_traffics.push_front(traffic);
	//}
	//else		//在历史流量列表的第1个元素中加上今天使用的流量
	//{
	//	m_history_traffics[0].kBytes = m_today_traffic / 1024;
	//}
	ofstream file{ theApp.m_history_traffic_path };
	for (const auto& history_traffic : m_history_traffics)
	{
		char date_str[16];
		sprintf_s(date_str, "%.4d/%.2d/%.2d ", history_traffic.year, history_traffic.month, history_traffic.day);
		file << date_str << history_traffic.kBytes << std::endl;
	}
}

void CTrafficMonitorDlg::LoadHistoryTraffic()
{
	ifstream file{ theApp.m_history_traffic_path };
	string current_line, temp;
	HistoryTraffic traffic;
	if (CCommon::FileExist(theApp.m_history_traffic_path.c_str()))
	{
		while (!file.eof())
		{
			if (m_history_traffics.size() > 999) break;		//最多读取1000天的历史记录
			std::getline(file, current_line);
			if (current_line.size() < 12) continue;
			temp = current_line.substr(0, 4);
			traffic.year = atoi(temp.c_str());
			temp = current_line.substr(5, 2);
			traffic.month = atoi(temp.c_str());
			temp = current_line.substr(8, 2);
			traffic.day = atoi(temp.c_str());
			temp = current_line.substr(11);
			traffic.kBytes = atoi(temp.c_str());
			m_history_traffics.push_back(traffic);
		}
	}

	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	traffic.year = current_time.wYear;
	traffic.month = current_time.wMonth;
	traffic.day = current_time.wDay;
	traffic.kBytes = 0;

	if (m_history_traffics.empty())
	{
		m_history_traffics.push_front(traffic);
	}

	if (m_history_traffics.size() >= 2)
	{
		//将读取到的历史流量列表按日期从大到小排序
		std::sort(m_history_traffics.begin(), m_history_traffics.end(), HistoryTraffic::DateGreater);

		//如果列表中有相同日期的项目，则将它合并
		for (int i{}; i < m_history_traffics.size() - 1; i++)
		{
			if (HistoryTraffic::DateEqual(m_history_traffics[i], m_history_traffics[i + 1]))
			{
				m_history_traffics[i].kBytes += m_history_traffics[i + 1].kBytes;
				m_history_traffics.erase(m_history_traffics.begin() + i + 1);
			}
		}
	}

	//如果列表第一个项目的日期是今天，则将第一个项目统计的流量作为今天使用的流量，否则，在列表的前面插入一个日期为今天的项目
	if (HistoryTraffic::DateEqual(m_history_traffics[0], traffic))
		theApp.m_today_traffic = static_cast<__int64>(m_history_traffics[0].kBytes) * 1024;
	else
		m_history_traffics.push_front(traffic);

}

// CTrafficMonitorDlg 消息处理程序

BOOL CTrafficMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	SetWindowText(_T("TrafficMonitor"));
	//设置隐藏任务栏图标
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	//设置窗口透明度
	SetTransparency();

	//m_menu.LoadMenu(IDR_MENU1); //装载右键菜单

	IniConnection();	//初始化连接
	//m_restart_cnt = 0;

	//如果启动时设置了鼠标穿透或隐藏主窗口，并且没有显示任务栏窗口，则显示通知区图标
	if ((m_mouse_penetrate || theApp.m_hide_main_window) && !m_show_task_bar_wnd)
		theApp.m_show_notify_icon = true;

	//设置通知区域图标
	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);	//该结构体变量的大小
	//m_ntIcon.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); //图标，通过资源ID得到
	m_ntIcon.hIcon = AfxGetApp()->LoadIcon(IDI_NOFITY_ICON);
	m_ntIcon.hWnd = this->m_hWnd;				//接收托盘图标通知消息的窗口句柄
	TCHAR atip[128] = _T("流量监控器");			//鼠标指向图标时显示的提示
	wcscpy_s(m_ntIcon.szTip, 128, atip);
	m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON;	//应用程序定义的消息ID号
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;	//图标的属性：设置成员uCallbackMessage、hIcon、szTip有效
	if (theApp.m_show_notify_icon)
		::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);	//在系统通知区域增加这个图标

	//载入流量历史记录
	LoadHistoryTraffic();

	//设置1000毫秒触发的定时器
	SetTimer(MAIN_TIMER, 1000, NULL);

	//初始化窗口位置
	CRect rect;
	GetWindowRect(rect);
	if (m_position_x != -1 && m_position_y != -1)
		rect.MoveToXY(m_position_x, m_position_y);

	m_window_height = rect.Height();
	//不显示CPU和内存利用率时的窗口高度为原来的64%（加上0.5用于补偿float转换为int时不能四舍五入导致的误差）
	//由于在不同DPI设置的电脑中运行时窗口的大小不是确定的，所有只能按比例计算
	m_window_height_s = static_cast<int>(rect.Height() * 0.64f + 0.5f);	

	if (!m_show_cpu_memory)
		rect.bottom = rect.top + m_window_height_s;
	MoveWindow(rect);
	CheckWindowPos();

	//初始化皮肤
	CCommon::GetFiles(L".\\skins", m_skins);
	if (m_skins.empty())
		m_skins.push_back(L"");
	//if (m_skin_selected < 0)
	//	m_skin_selected = 0;
	//if (m_skin_selected >= m_skins.size())
	//	m_skin_selected = m_skins.size() - 1;
	m_skin_selected = 0;
	for (unsigned int i{}; i<m_skins.size(); i++)
	{
		if (m_skins[i] == m_skin_name)
			m_skin_selected = i;
	}

	//载入背景图片
	if (m_show_cpu_memory)
		m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height, LR_LOADFROMFILE);
	else
		m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height_s, LR_LOADFROMFILE);
	SetBackgroundImage(m_back_img);

	//设置文字颜色
	m_disp_cpu.SetTextColor(m_text_color);
	m_disp_memory.SetTextColor(m_text_color);
	m_disp_up.SetTextColor(m_text_color);
	m_disp_down.SetTextColor(m_text_color);

	//设置字体
	m_font.CreatePointFont(m_font_size * 10, m_font_name);
	m_disp_cpu.SetFont(&m_font);
	m_disp_memory.SetFont(&m_font);
	m_disp_up.SetFont(&m_font);
	m_disp_down.SetFont(&m_font);


	//获取“启动”文件夹的路径
	m_start_up_path = CCommon::GetStartUpPath();
	//判断程序的自启动快捷方式是否存在
	m_auto_run = CCommon::FileExist((m_start_up_path + L"\\TrafficMonitor.lnk").c_str());

	//m_WM_TASKBARCREATED = ::RegisterWindowMessage(_T("TaskbarCreated"));

	//获取启动时的时间
	GetLocalTime(&m_start_time);

	m_tool_tips.Create(this, TTS_ALWAYSTIP);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

//void CTrafficMonitorDlg::OnPaint()
//{
//	if (IsIconic())
//	{
//		CPaintDC dc(this); // 用于绘制的设备上下文
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// 使图标在工作区矩形中居中
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// 绘制图标
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		CDialogEx::OnPaint();
//	}
//}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTrafficMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



//LRESULT CTrafficMonitorDlg::OnNcHitTest(CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	//return HTCAPTION;
//	return CDialogEx::OnNcHitTest(point);
//}


void CTrafficMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == MAIN_TIMER)
	{
		if (m_first_start)		//这个if语句在程序启动后1秒执行
		{
			//将设置窗口置顶的处理放在这里是用于解决
			//放在初始化函数中可能会出现设置置顶无效的问题
			SetAlwaysOnTop();		//设置窗口置顶
			SetMousePenetrate();	//设置鼠标穿透
			if (theApp.m_hide_main_window)	//设置隐藏主窗口
				ShowWindow(SW_HIDE);
			
			//显示任务栏窗口
			if (m_show_task_bar_wnd && m_tBarDlg == nullptr)
				OpenTaskBarWnd();

			m_first_start = false;
		}

		//程序启动后若干秒的时候根据设置重新执行“总是置顶”、“鼠标穿透”和“隐藏主窗口”的操作，防止设置没有生效
		//if (m_timer_cnt % 30 == 4)
		if (m_timer_cnt == 5 || m_timer_cnt == 9)
		{
			if (!theApp.m_hide_main_window)
			{
				SetAlwaysOnTop();
				SetMousePenetrate();
			}
			else
			{
				ShowWindow(SW_HIDE);
			}
		}

		if (m_timer_cnt % 300 == 299 && !theApp.m_hide_main_window)
		{
			SetAlwaysOnTop();		//每5分钟执行一次设置窗口置顶
		}

		//只有主窗口和任务栏窗口至少有一个显示时才执行下面的处理
		if (!theApp.m_hide_main_window || m_show_task_bar_wnd)
		{
			//获取网络连接速度
			int rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
			m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets;
			m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets;

			//如果发送和接收的字节数为0或上次发送和接收的字节数为0或当前连接已改变时，网速无效
			if ((m_in_bytes == 0 && m_out_bytes == 0) || (m_last_in_bytes == 0 && m_last_out_bytes) || m_connection_change_flag)
			{
				theApp.m_in_speed = 0;
				theApp.m_out_speed = 0;
			}
			else
			{
				theApp.m_in_speed = m_in_bytes - m_last_in_bytes;
				theApp.m_out_speed = m_out_bytes - m_last_out_bytes;
			}
			//如果大于1GB/s，说明可能产生了异常，网速无效
			if (theApp.m_in_speed > 1073741824)
				theApp.m_in_speed = 0;
			if (theApp.m_out_speed > 1073741824)
				theApp.m_out_speed = 0;

			m_connection_change_flag = false;	//清除连接发生变化的标志

			m_last_in_bytes = m_in_bytes;
			m_last_out_bytes = m_out_bytes;

			//处于自动选择状态时，如果连续30秒没有网速，则可能自动选择的网络不对，此时执行一次自动选择
			if (m_auto_select)
			{
				if (theApp.m_in_speed == 0 && theApp.m_out_speed == 0)
					m_zero_speed_cnt++;
				else
					m_zero_speed_cnt = 0;
				if (m_zero_speed_cnt >= 30)
				{
					AutoSelect();
					m_zero_speed_cnt = 0;
				}
			}

			//检测当前日期是否改变，如果已改变，就向历史流量列表插入一个新的日期
			SYSTEMTIME current_time;
			GetLocalTime(&current_time);
			if (m_history_traffics[0].day != current_time.wDay)
			{
				HistoryTraffic traffic;
				traffic.year = current_time.wYear;
				traffic.month = current_time.wMonth;
				traffic.day = current_time.wDay;
				traffic.kBytes = 0;
				m_history_traffics.push_front(traffic);
				theApp.m_today_traffic = 0;
			}

			//统计今天已使用的流量
			theApp.m_today_traffic += (theApp.m_in_speed + theApp.m_out_speed);
			m_history_traffics[0].kBytes = static_cast<unsigned int>(theApp.m_today_traffic / 1024);
			//每隔30秒保存一次流量历史记录
			if (m_timer_cnt % 30 == 10)
				SaveHistoryTraffic();

			char buff[256];
			if (rtn == ERROR_INSUFFICIENT_BUFFER)
			{
				IniConnection();
				sprintf_s(buff, "用于储存连接信息的缓冲区大小不够，已重新初始化连接。(已重新初始化%d次)", m_restart_cnt);
				CCommon::WriteLog(buff, theApp.m_log_path.c_str());
			}
			//else if(m_pIfTable->table[m_connections[m_connection_selected].index].dwOperStatus != IF_OPER_STATUS_OPERATIONAL)
			//{
			//	sprintf_s(buff, "当前连接%s已断开，已重新初始化连接。(已重新初始化%d次)", (const char*)m_pIfTable->table[m_connections[m_connection_selected].index].bDescr, m_restart_cnt);
			//	CCommon::WriteLog(buff, LOG_PATH);
			//	IniConnection();
			//}
			//else if (m_restart_cnt <= MAX_RESTART_CNT)
			//{
			//	if ((m_in_bytes == 0 && m_out_bytes == 0) || m_in_speed > 2147483648U || m_out_speed > 2147483648U)
			//	{
			//		IniConnection();
			//		sprintf_s(buff, "可能出现了异常，已重新初始化连接。(已发送字节数：%u，已接收字节数：%u，上传速度：%u，下载速度：%u，当前连接：%s，已重新初始化%d次)", \
			//			m_out_bytes, m_in_bytes, m_out_speed, m_in_speed, (const char*)m_pIfTable->table[m_connections[m_connection_selected].index].bDescr, m_restart_cnt);
			//		CCommon::WriteLog(buff, LOG_PATH);
			//	}
			//}

			//统计当前已发送或已接收字节数不为0的连接个数
			int connection_count{};
			string descr;
			for (unsigned int i{}; i < m_pIfTable->dwNumEntries; i++)
			{
				descr = (const char*)m_pIfTable->table[i].bDescr;
				if (m_pIfTable->table[i].dwInOctets > 0 || m_pIfTable->table[i].dwOutOctets > 0 || descr == m_connection_name)
					connection_count++;
			}
			if (connection_count == 0) connection_count = 1;
			if (connection_count != m_connections.size())	//如果连接数发生变化，则重新初始化连接
			{
				sprintf_s(buff, "检测到连接数发生变化，已重新获取连接。先前连接数：%d，现在连接数：%d。(已重新初始化%d次)", m_connections.size(), connection_count, m_restart_cnt + 1);
				IniConnection();
				CCommon::WriteLog(buff, theApp.m_log_path.c_str());
			}
			descr = (const char*)m_pIfTable->table[m_connections[m_connection_selected].index].bDescr;
			if (descr != m_connection_name)
			{
				IniConnection();
				sprintf_s(buff, "可能出现了异常，当前选择的连接和期望的连接不一致，已重新获取连接。(已重新初始化%d次)", m_restart_cnt);
				CCommon::WriteLog(buff, theApp.m_log_path.c_str());
			}


			if (m_show_cpu_memory || (m_tBarDlg != nullptr && m_tBarDlg->m_show_cpu_memory))
			{
				//获取CPU利用率
				FILETIME idleTime;
				FILETIME kernelTime;
				FILETIME userTime;
				GetSystemTimes(&idleTime, &kernelTime, &userTime);

				__int64 idle = CCommon::CompareFileTime2(m_preidleTime, idleTime);
				__int64 kernel = CCommon::CompareFileTime2(m_prekernelTime, kernelTime);
				__int64 user = CCommon::CompareFileTime2(m_preuserTime, userTime);

				if (kernel + user == 0)
				{
					theApp.m_cpu_usage = 0;
				}
				else
				{
					//（总的时间-空闲时间）/总的时间=占用cpu的时间就是使用率
					theApp.m_cpu_usage = static_cast<int>(abs((kernel + user - idle) * 100 / (kernel + user)));
				}
				m_preidleTime = idleTime;
				m_prekernelTime = kernelTime;
				m_preuserTime = userTime;
		
				//获取内存利用率
				MEMORYSTATUSEX statex;
				statex.dwLength = sizeof(statex);
				GlobalMemoryStatusEx(&statex);
				theApp.m_memory_usage = statex.dwMemoryLoad;
				theApp.m_used_memory = static_cast<int>((statex.ullTotalPhys - statex.ullAvailPhys) / 1024);
				theApp.m_total_memory  = static_cast<int>(statex.ullTotalPhys / 1024);
			}

			ShowInfo();		//刷新窗口信息
	
			////如果检测到鼠标光标在窗口外面，就将m_tip_type置为0
			//CPoint cursor_point;
			//GetCursorPos(&cursor_point);
			//CRect rect;
			//GetWindowRect(rect);
			//if (rect.PtInRect(cursor_point) == 0)
			//	m_tip_type = 0;

			//更新任务栏窗口信息
			if (m_tBarDlg != nullptr)
			{
				//m_tBarDlg->m_in_speed = m_in_speed;
				//m_tBarDlg->m_out_speed = m_out_speed;
				//m_tBarDlg->m_cpu_usage = m_cpu_usage;
				//m_tBarDlg->m_memory_usage = m_memory_usage;
				//m_tBarDlg->m_used_memory = m_used_memory;
				//m_tBarDlg->m_total_memory = m_total_memory;
				//m_tBarDlg->m_today_traffic = m_today_traffic;
				//在任务栏窗口显示期间，随时调整任务栏窗口的位置，防止任务栏图标遮挡窗口
				m_tBarDlg->AdjustWindowPos();
				m_tBarDlg->ShowInfo();
				//如果任务栏窗口出现异常，则删除任务栏窗口再重新显示
				//if (!m_tBarDlg->AdjustWindowPos())
				//{
				//	m_no_task_bar = true;
				//}
				////直到任务栏资源管理器重启完成，任务栏显示出来时，才重新显示任务栏窗口，在此期间m_no_task_bar为true
				//if (m_no_task_bar && CCommon::TaskBarExist())
				//{
				//	CloseTaskBarWnd();
				//	if (m_show_notify_icon)
				//	{
				//		//重新添加通知栏图标
				//		::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
				//	}
				//	OpenTaskBarWnd();
				//	m_no_task_bar = false;
				//}
			}
			//else		//没有显示任务栏窗口时，在资源管理器重启后重新添加通知区图标
			//{
			//	if (!CCommon::TaskBarExist())		//资源管理器进程结束时，任务栏不存在
			//	{
			//		m_no_task_bar = true;
			//	}
			//	if (m_no_task_bar && CCommon::TaskBarExist())	//资源管理器重启完成时，重新添加通知区图标
			//	{
			//		if (m_show_notify_icon)
			//			::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
			//		m_no_task_bar = false;
			//	}
			//}
		}

		m_timer_cnt++;
		//if (m_timer_cnt >= 120)
		//{
		//	m_timer_cnt = 0;
		//	m_restart_cnt = 0;		//每隔2分钟重新初始化次数置0
		//}
	}

	if (nIDEvent == DELAY_TIMER)
	{
		AutoSelect();
		KillTimer(DELAY_TIMER);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CTrafficMonitorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//设置点击鼠标右键弹出菜单
	CMenu* pContextMenu = m_menu.GetSubMenu(0); //获取第一个弹出菜单，所以第一个菜单必须有子菜单 
	CPoint point1;	//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
	pContextMenu->SetDefaultItem(ID_NETWORK_INFO);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTrafficMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//在未锁定窗口位置时允许通过点击窗口内部来拖动窗口
	if (!m_lock_window_pos)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CTrafficMonitorDlg::OnNetworkInfo()
{
	// TODO: 在此添加命令处理程序代码
	//弹出“连接详情”对话框
	CNetworkInfoDlg aDlg(m_pIfTable->table[m_connections[m_connection_selected].index]);
	//向CNetworkInfoDlg类传递自启动以来已发送和接收的字节数
	aDlg.m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets - m_connections[m_connection_selected].in_bytes;
	aDlg.m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets - m_connections[m_connection_selected].out_bytes;
	aDlg.m_start_time = m_start_time;
	aDlg.DoModal();
	SetAlwaysOnTop();	//由于在“连接详情”对话框内设置了取消窗口置顶，所有在对话框关闭后，重新设置窗口置顶
	if(m_tBarDlg!=nullptr)
		m_tBarDlg->m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);	//重新设置任务栏窗口的提示信息置顶
}


void CTrafficMonitorDlg::OnAlwaysOnTop()
{
	// TODO: 在此添加命令处理程序代码
	m_always_on_top = !m_always_on_top;
	SetAlwaysOnTop();
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateAlwaysOnTop(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_always_on_top);
}


void CTrafficMonitorDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialogEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: 在此处添加消息处理程序代码
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child Windows dont have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup cant be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}


void CTrafficMonitorDlg::OnTransparency100()
{
	// TODO: 在此添加命令处理程序代码
	m_transparency = 100;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency80()
{
	// TODO: 在此添加命令处理程序代码
	m_transparency = 80;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency60()
{
	// TODO: 在此添加命令处理程序代码
	m_transparency = 60;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency40()
{
	// TODO: 在此添加命令处理程序代码
	m_transparency = 40;
	SetTransparency();
	SaveConfig();
}


//void CTrafficMonitorDlg::OnUpdateTransparency100(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_transparency == 100);
//}


//void CTrafficMonitorDlg::OnUpdateTransparency80(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_transparency == 80);
//}


//void CTrafficMonitorDlg::OnUpdateTransparency60(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_transparency == 60);
//}


//void CTrafficMonitorDlg::OnUpdateTransparency40(CCmdUI *pCmdUI)
//{
//	// TODO: 在此添加命令更新用户界面处理程序代码
//	pCmdUI->SetCheck(m_transparency == 40);
//}


void CTrafficMonitorDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_cannot_save_config_warning = true;
	SaveConfig();	//退出前保存设置到ini文件
	SaveHistoryTraffic();

	if (m_tBarDlg != nullptr)
		m_tBarDlg->OnCancel();

	//const int WIND_NUM{ 5 };
	//const CString diloge_title[WIND_NUM]{ _T("关于 TrafficMonitor"),_T("捐助"), _T("历史流量统计"), _T("连接详情"), _T("更换皮肤") };
	//HWND handle{};
	//for (int i{}; i < WIND_NUM; i++)
	//{
	//	while (true)
	//	{
	//		handle = ::FindWindow(NULL, diloge_title[i]);
	//		if (handle != NULL)
	//			::EndDialog(handle, IDOK);
	//		else
	//			break;
	//	}
	//}

	CDialogEx::OnClose();
}


//BOOL CTrafficMonitorDlg::PreCreateWindow(CREATESTRUCT& cs)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	if (m_position_x >= 0 && m_position_y >= 0)
//	{
//		cs.x = m_position_x;
//		cs.y = m_position_y;
//	}
//	return CDialogEx::PreCreateWindow(cs);
//}


BOOL CTrafficMonitorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT uMsg = LOWORD(wParam);
	//选择了“选择网络连接”子菜单中项目时的处理
	if (uMsg == ID_SELETE_CONNECTION)	//选择了“自动选择”菜单项
	{
		AutoSelect();
		m_auto_select = true;
		SaveConfig();
		m_connection_change_flag = true;
	}
	if (uMsg > ID_SELETE_CONNECTION && uMsg <= ID_SELETE_CONNECTION + m_connections.size())	//选择了一个网络连接
	{
		m_connection_selected = uMsg - ID_SELETE_CONNECTION - 1;
		m_connection_name = m_connections[m_connection_selected].description;
		m_auto_select = false;
		SaveConfig();
		m_connection_change_flag = true;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CTrafficMonitorDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	//设置“选择连接”子菜单项中各单选项的选择状态
	if (m_auto_select)		//m_auto_select为true时为自动选择，选中菜单的第1项
		m_select_connection_menu->CheckMenuRadioItem(0, m_connections.size(), 0, MF_BYPOSITION | MF_CHECKED);
	else		//m_auto_select为false时非自动选择，根据m_connection_selected的值选择对应的项
		m_select_connection_menu->CheckMenuRadioItem(0, m_connections.size(), m_connection_selected + 1, MF_BYPOSITION | MF_CHECKED);

	//设置“窗口不透明度”子菜单下各单选项的选择状态
	switch (m_transparency)
	{
	case 100: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_100, MF_BYCOMMAND | MF_CHECKED); break;
	case 80: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_80, MF_BYCOMMAND | MF_CHECKED); break;
	case 60: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_60, MF_BYCOMMAND | MF_CHECKED); break;
	case 40: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_40, MF_BYCOMMAND | MF_CHECKED); break;
	default: break;
	}

	//if (m_mouse_penetrate || (theApp.m_hide_main_window&&!m_show_task_bar_wnd))	//如果启动时设置了鼠标穿透或隐藏主窗口，禁用“显示通知区图标”菜单项
	if(!m_show_task_bar_wnd && (theApp.m_hide_main_window || m_mouse_penetrate))	//如果没有显示任务栏窗口，且隐藏了主窗口或设置了鼠标穿透，则禁用“显示通知区图标”菜单项
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);
	else
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);

	//pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


//int CTrafficMonitorDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  在此添加您专用的创建代码
//	if (!RegisterTouchWindow())
//	{
//		ASSERT(FALSE); 
//	}
//	return 0;
//}


BOOL CTrafficMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽按回车键和ESC键退出
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	if (m_tool_tips.GetSafeHwnd())
	{
		m_tool_tips.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTrafficMonitorDlg::OnLockWindowPos()
{
	// TODO: 在此添加命令处理程序代码
	m_lock_window_pos = !m_lock_window_pos;
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateLockWindowPos(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_lock_window_pos);
}


void CTrafficMonitorDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: 在此处添加消息处理程序代码
	//确保窗口不会超出屏幕范围
	CheckWindowPos();
}


//void CTrafficMonitorDlg::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	CheckWindowPos();
//	CDialogEx::OnLButtonUp(nFlags, point);
//}


afx_msg LRESULT CTrafficMonitorDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	bool dialog_exist{ false };
	const int WIND_NUM{ 5 };
	const CString diloge_title[WIND_NUM]{ _T("关于 TrafficMonitor"),_T("捐助"), _T("历史流量统计"), _T("连接详情"), _T("更换皮肤") };
	HWND handle{};
	//依次查找程序中的每一个对话框，如果找到一个没有关闭的对话框时，则不允许弹出右键菜单，防止用户在此时退出程序
	for (int i{}; i < WIND_NUM; i++)
	{
		handle = ::FindWindow(NULL, diloge_title[i]);
		if (handle != NULL)
		{
			dialog_exist = true;
			break;
		}
	}

	if (lParam == WM_LBUTTONDOWN)
	{
		if (!theApp.m_hide_main_window)
		{
			if (dialog_exist)		//有打开的对话框时，点击通知区图标后将焦点设置到对话框
			{
				::SetForegroundWindow(handle);
			}
			else		//没有打开的对话框时，则显示主窗口
			{
				ShowWindow(SW_RESTORE);
				theApp.m_hide_main_window = false;
				//if (m_tBarDlg != nullptr)
				//	m_tBarDlg->m_hide_main_window = false;	//同时将m_hide_main_window的值也设置false
				SetForegroundWindow();
				//GetScreenSize(34);
				CheckWindowPos();
				SaveConfig();
			}
		}
	}
	if (lParam == WM_RBUTTONUP && !dialog_exist)
	{
		//在通知区点击右键弹出右键菜单
		if (m_tBarDlg != nullptr)		//如果显示了任务栏窗口，则在右击了通知区图标后将焦点设置到任务栏窗口
			m_tBarDlg->SetForegroundWindow();
		else				//否则将焦点设置到主窗口
			SetForegroundWindow();
		//OnRButtonUp(0, CPoint());
		CPoint point1;	//定义一个用于确定光标位置的位置  
		GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
		m_menu.GetSubMenu(0)->SetDefaultItem(-1);		//设置没有默认菜单项
		m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

		CheckWindowPos();
	}
	if (lParam == WM_LBUTTONDBLCLK)
	{
		////在通知区双击左键弹出“连接详情”对话框
		//handle = ::FindWindow(NULL, _T("连接详情"));
		////如果“连接详情”已经存在，则将其设置为焦点，否则，弹出“连接详情”对话框
		//if (handle != NULL)
		//	::SetForegroundWindow(handle);
		//else
		//	OnNetworkInfo();

		if (dialog_exist)		//有打开的对话框时，点击通知区图标后将焦点设置到对话框
		{
			::SetForegroundWindow(handle);
		}
		else		//没有打开的对话框时，则显示主窗口
		{
			ShowWindow(SW_RESTORE);
			theApp.m_hide_main_window = false;
			//if (m_tBarDlg != nullptr)
			//	m_tBarDlg->m_hide_main_window = false;	//同时将m_hide_main_window的值也设置false
			SetForegroundWindow();
			//GetScreenSize(34);
			CheckWindowPos();
			SaveConfig();
		}
	}
	return 0;
}


void CTrafficMonitorDlg::OnShowNotifyIcon()
{
	// TODO: 在此添加命令处理程序代码
	if (theApp.m_show_notify_icon)
	{
		DeleteNotifyIcon();
		theApp.m_show_notify_icon = false;
	}
	else
	{
		AddNotifyIcon();
		theApp.m_show_notify_icon = true;
	}
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateShowNotifyIcon(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.m_show_notify_icon);
}


void CTrafficMonitorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	//程序退出时删除通知栏图标
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
	// TODO: 在此处添加消息处理程序代码
}


void CTrafficMonitorDlg::OnShowCpuMemory()
{
	// TODO: 在此添加命令处理程序代码
	CRect rect;
	GetWindowRect(rect);
	if (m_show_cpu_memory)
	{
		rect.bottom = rect.top + m_window_height_s;
		MoveWindow(rect);
		CheckWindowPos();
		m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height_s, LR_LOADFROMFILE);
		m_show_cpu_memory = false;
	}
	else
	{
		rect.bottom = rect.top + m_window_height;
		MoveWindow(rect);
		CheckWindowPos();
		m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height, LR_LOADFROMFILE);
		m_show_cpu_memory = true;
	}
	SetBackgroundImage(m_back_img);
	ShowInfo();
	SaveConfig();
}


//任务栏窗口切换显示CPU和内存利用率时的处理
void CTrafficMonitorDlg::OnShowCpuMemory2()
{
	// TODO: 在此添加命令处理程序代码
	if (m_tBarDlg != nullptr)
	{
		m_tBarDlg->m_show_cpu_memory = !m_tBarDlg->m_show_cpu_memory;
		//切换显示CPU和内存利用率时，删除任务栏窗口，再重新显示
		CloseTaskBarWnd();
		OpenTaskBarWnd();
	}
}


void CTrafficMonitorDlg::OnUpdateShowCpuMemory(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_show_cpu_memory);
}


void CTrafficMonitorDlg::OnMousePenetrate()
{
	// TODO: 在此添加命令处理程序代码
	m_mouse_penetrate = !m_mouse_penetrate;
	SetMousePenetrate();
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateMousePenetrate(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_mouse_penetrate);
}


void CTrafficMonitorDlg::OnTextColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog colorDlg(m_text_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_text_color = colorDlg.GetColor();
		m_disp_cpu.SetTextColor(m_text_color);
		m_disp_memory.SetTextColor(m_text_color);
		m_disp_up.SetTextColor(m_text_color);
		m_disp_down.SetTextColor(m_text_color);
		SaveConfig();
	}
}


void CTrafficMonitorDlg::OnShowTaskBarWnd()
{
	// TODO: 在此添加命令处理程序代码
	if (m_tBarDlg != nullptr)
	{
		CloseTaskBarWnd();
	}
	if (!m_show_task_bar_wnd)
	{
		m_show_task_bar_wnd = true;
		OpenTaskBarWnd();
	}
	else
	{
		m_show_task_bar_wnd = false;
		//关闭任务栏窗口后，如果没有显示通知区图标，且没有显示主窗口或设置了鼠标穿透，则将通知区图标显示出来
		if (!theApp.m_show_notify_icon && (theApp.m_hide_main_window || m_mouse_penetrate))
		{
			AddNotifyIcon();
			theApp.m_show_notify_icon = true;
		}
	}
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateShowTaskBarWnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_show_task_bar_wnd);
}


void CTrafficMonitorDlg::OnAppAbout()
{
	// TODO: 在此添加命令处理程序代码
	//弹出“关于”对话框
	CAboutDlg aDlg;
	aDlg.DoModal();
}



void CTrafficMonitorDlg::OnAutoRunWhenStart()
{
	// TODO: 在此添加命令处理程序代码
	if (!m_auto_run)
	{
		if (MessageBox(_T("是否要在“开始”菜单的“启动”目录下创建程序的快捷方式？"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (CCommon::CreateFileShortcut(m_start_up_path.c_str(), NULL, _T("TrafficMonitor.lnk")))
			{
				CString info;
				info.Format(_T("已经在“%s”路径下创建了程序的快捷方式。"), m_start_up_path.c_str());
				MessageBox(info, NULL, MB_ICONINFORMATION);
				m_auto_run = true;
			}
			else
			{
				MessageBox(_T("快捷方式创建失败！"), NULL, MB_ICONWARNING);
			}
		}
	}
	else
	{
		if (MessageBox(_T("是否要在“开始”菜单的“启动”目录下删除程序的快捷方式？"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (DeleteFile((m_start_up_path + L"\\TrafficMonitor.lnk").c_str()))
			{
				MessageBox(_T("快捷方式删除成功！"), NULL, MB_ICONINFORMATION);
				m_auto_run = false;
			}
			else
			{
				MessageBox(_T("快捷方式删除失败！"), NULL, MB_ICONWARNING);
			}
		}
	}
}


void CTrafficMonitorDlg::OnUpdateAutoRunWhenStart(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_auto_run);
}


//当资源管理器重启时会触发此消息
LRESULT CTrafficMonitorDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (m_tBarDlg != nullptr)
	{
		CloseTaskBarWnd();
		if (theApp.m_show_notify_icon)
		{
			//重新添加通知栏图标
			::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
		}
		OpenTaskBarWnd();
	}
	else
	{
		if (theApp.m_show_notify_icon)
			::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
	}
	return LRESULT();
}


void CTrafficMonitorDlg::OnHideMainWnd()
{
	// TODO: 在此添加命令处理程序代码
	if (!theApp.m_hide_main_window)
	{
		ShowWindow(SW_HIDE);
		theApp.m_hide_main_window = true;
		//隐藏主窗口后，如果没有显示通知栏图标，则将其显示出来
		if (!theApp.m_show_notify_icon && !m_show_task_bar_wnd)
		{
			AddNotifyIcon();
			theApp.m_show_notify_icon = true;
		}
		////禁用“显示通知栏图标”菜单项
		//m_menu.EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		ShowWindow(SW_RESTORE);
		theApp.m_hide_main_window = false;
		//if (!m_mouse_penetrate)		//在取消“隐藏主窗口”的选项时，只有没有选中“鼠标穿透”才恢复“显示通知区图标”菜单为可用
		//	m_menu.EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);
	}
	//if (m_tBarDlg != nullptr)
	//	m_tBarDlg->m_hide_main_window = m_hide_main_window;		//将m_hide_main_window的值传递给m_tBarDlg里的变量
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateHideMainWnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.m_hide_main_window);
}


void CTrafficMonitorDlg::OnChangeSkin()
{
	// TODO: 在此添加命令处理程序代码
	CRect rect;
	GetWindowRect(rect);
	CSkinDlg skinDlg;
	//初始化CSkinDlg对象的数据
	skinDlg.m_skins = m_skins;
	skinDlg.m_skin_selected = m_skin_selected;
	skinDlg.m_skin_width = rect.Width();
	skinDlg.m_skin_height_s = m_window_height_s;
	skinDlg.m_skin_height_l = m_window_height;
	if (skinDlg.DoModal() == IDOK)
	{
		m_skin_selected = skinDlg.m_skin_selected;
		m_skin_name = m_skins[m_skin_selected];
		//载入背景图片
		if (m_show_cpu_memory)
			m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height, LR_LOADFROMFILE);
		else
			m_back_img = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, rect.Width(), m_window_height_s, LR_LOADFROMFILE);
		SetBackgroundImage(m_back_img);
		//获取皮肤的文字颜色，并设置文字颜色
		m_text_color = skinDlg.GetTextColor();
		m_disp_cpu.SetTextColor(m_text_color);
		m_disp_memory.SetTextColor(m_text_color);
		m_disp_up.SetTextColor(m_text_color);
		m_disp_down.SetTextColor(m_text_color);

		SaveConfig();
	}
}



void CTrafficMonitorDlg::OnSetFont()
{
	// TODO: 在此添加命令处理程序代码
	LOGFONT lf{};             //LOGFONT变量
	m_font.GetLogFont(&lf);
	CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
	if (IDOK == fontDlg.DoModal())     // 显示字体对话框
	{
		//如果m_font已经关联了一个字体资源对象，则释放它
		if (m_font.m_hObject)
		{
			m_font.DeleteObject();
		}
		//使用选定字体的LOGFONT创建新的字体
		m_font.CreateFontIndirect(fontDlg.m_cf.lpLogFont);
		//设置字体
		m_disp_cpu.SetFont(&m_font);
		m_disp_memory.SetFont(&m_font);
		m_disp_up.SetFont(&m_font);
		m_disp_down.SetFont(&m_font);
		//获取字体信息
		m_font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
		m_font_size = fontDlg.m_cf.iPointSize / 10;
		//将字体设置写入到ini文件
		SaveConfig();
	}
}


void CTrafficMonitorDlg::OnTrafficHistory()
{
	// TODO: 在此添加命令处理程序代码
	CHistoryTrafficDlg historyDlg(m_history_traffics);
	historyDlg.DoModal();
}


void CTrafficMonitorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_tool_tips.SetMaxTipWidth(600);
	CString tip_info;
	if (m_show_cpu_memory)
	{
		tip_info.Format(_T("今日已使用流量：%s\r\n内存使用：%s/%s"), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_traffic / 1024)),
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
	}
	else
	{
		tip_info.Format(_T("今日已使用流量：%s\r\nCPU使用：%d%%\r\n内存使用：%s/%s (%d%%)"), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_traffic / 1024)),
			theApp.m_cpu_usage,
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory),
			theApp.m_memory_usage);
	}
	m_tool_tips.AddTool(this, tip_info);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTrafficMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	OnNetworkInfo();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
