
// TrafficMonitorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������

protected:
	CPictureStatic m_about_img;		//���ڶԻ����е�ͼƬ
	CStaticEx m_mail{ true };	//����ϵ���ߡ�������
	CStaticEx m_check_update{ true };	//�������¡�������
	CStaticEx m_github{ true };		//��GitHub��������
	CStaticEx m_donate{ true };	//��������������
	CToolTipCtrl m_Mytip;		//���ָ��ʱ�Ĺ�����ʾ
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnStnClickedStaticDonate();
protected:
	afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//ON_STN_CLICKED(IDC_STATIC_DONATE, &CAboutDlg::OnStnClickedStaticDonate)
	ON_MESSAGE(WM_LINK_CLICKED, &CAboutDlg::OnLinkClicked)
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
	DDX_Control(pDX, IDC_STATIC_GITHUB, m_github);
	DDX_Control(pDX, IDC_STATIC_DONATE, m_donate);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRect rect;
	GetClientRect(rect);
	rect.bottom = rect.Height() * 2 / 5;	//ͼƬ�߶�ռ�Ի���߶ȵ�2/5
	m_about_img.MoveWindow(rect);
	if (theApp.DPI(100) >= 125)
		//m_about_img.SetBitmap(LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_ABOUT_BACKGROUND_HD)));
		m_about_img.SetPicture(IDB_ABOUT_BACKGROUND_HD);
	else
		//m_about_img.SetBitmap(LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(IDB_ABOUT_BACKGROUND)));
		m_about_img.SetPicture(IDB_ABOUT_BACKGROUND);
	m_mail.SetURL(_T("mailto:zhongyang219@hotmail.com"));	//���ó�����
	//m_check_update.SetURL(_T("http://pan.baidu.com/s/1c1LkPQ4"));
	m_github.SetURL(_T("https://github.com/zhongyang219/TrafficMonitor"));
	m_donate.SetLinkEnable(false);
	m_check_update.SetLinkEnable(false);

	//���ð汾��Ϣ
	CString version_info;
	GetDlgItemText(IDC_STATIC_VERSION, version_info);
	version_info.Replace(_T("<version>"), VERSION);
	SetDlgItemText(IDC_STATIC_VERSION, version_info);
	
	m_Mytip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_Mytip.AddTool(&m_mail, _T("�����߷��͵����ʼ�\r\nmailto:zhongyang219@hotmail.com"));
	//m_Mytip.AddTool(&m_check_update, _T("���ٶ��������Ӳ鿴�Ƿ��и���\r\nhttp://pan.baidu.com/s/1c1LkPQ4"));
	m_Mytip.AddTool(&m_github, _T("ת������Ŀ��GitHub�ϵ�ҳ��\r\nhttps://github.com/zhongyang219/TrafficMonitor"));
	m_Mytip.AddTool(&m_donate, _T("��������"));
	m_Mytip.SetDelayTime(300);	//�����ӳ�
	m_Mytip.SetMaxTipWidth(800);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_MOUSEMOVE)
		m_Mytip.RelayEvent(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

//void CAboutDlg::OnStnClickedStaticDonate()
//{
//	CDonateDlg donateDlg;
//	donateDlg.DoModal();
//}

afx_msg LRESULT CAboutDlg::OnLinkClicked(WPARAM wParam, LPARAM lParam)
{
	switch (::GetDlgCtrlID(((CWnd*)wParam)->m_hWnd))
	{
	case IDC_STATIC_DONATE:
		{
			CDonateDlg donateDlg;
			donateDlg.DoModal();
		}
		break;
	case IDC_STATIC_CHECK_UPDATE:
		theApp.CheckUpdate(true);
		break;
	}
	return 0;
}


// CTrafficMonitorDlg �Ի���

//��̬��Ա��ʼ��
unsigned int CTrafficMonitorDlg::m_WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };	//ע����������������Ϣ

CTrafficMonitorDlg::CTrafficMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAFFICMONITOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	LoadConfig();	//����ʱ��ini�ļ���������
	CTaskBarDlg::LoadConfig();
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
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
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
	ON_WM_CLOSE()
	ON_WM_INITMENU()
	ON_COMMAND(ID_LOCK_WINDOW_POS, &CTrafficMonitorDlg::OnLockWindowPos)
	ON_UPDATE_COMMAND_UI(ID_LOCK_WINDOW_POS, &CTrafficMonitorDlg::OnUpdateLockWindowPos)
	ON_WM_MOVE()
	ON_MESSAGE(MY_WM_NOTIFYICON, &CTrafficMonitorDlg::OnNotifyIcon)
	ON_COMMAND(ID_SHOW_NOTIFY_ICON, &CTrafficMonitorDlg::OnShowNotifyIcon)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NOTIFY_ICON, &CTrafficMonitorDlg::OnUpdateShowNotifyIcon)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SHOW_CPU_MEMORY, &CTrafficMonitorDlg::OnShowCpuMemory)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CPU_MEMORY, &CTrafficMonitorDlg::OnUpdateShowCpuMemory)
	ON_COMMAND(ID_MOUSE_PENETRATE, &CTrafficMonitorDlg::OnMousePenetrate)
	ON_UPDATE_COMMAND_UI(ID_MOUSE_PENETRATE, &CTrafficMonitorDlg::OnUpdateMousePenetrate)
	ON_COMMAND(ID_SHOW_TASK_BAR_WND, &CTrafficMonitorDlg::OnShowTaskBarWnd)
	ON_UPDATE_COMMAND_UI(ID_SHOW_TASK_BAR_WND, &CTrafficMonitorDlg::OnUpdateShowTaskBarWnd)
	ON_COMMAND(ID_APP_ABOUT, &CTrafficMonitorDlg::OnAppAbout)
	ON_COMMAND(ID_SHOW_CPU_MEMORY2, &CTrafficMonitorDlg::OnShowCpuMemory2)
	//ON_COMMAND(ID_AUTO_RUN_WHEN_START, &CTrafficMonitorDlg::OnAutoRunWhenStart)
	ON_COMMAND(ID_HIDE_MAIN_WND, &CTrafficMonitorDlg::OnHideMainWnd)
	ON_UPDATE_COMMAND_UI(ID_HIDE_MAIN_WND, &CTrafficMonitorDlg::OnUpdateHideMainWnd)
	ON_COMMAND(ID_CHANGE_SKIN, &CTrafficMonitorDlg::OnChangeSkin)
	//ON_UPDATE_COMMAND_UI(ID_AUTO_RUN_WHEN_START, &CTrafficMonitorDlg::OnUpdateAutoRunWhenStart)
	ON_REGISTERED_MESSAGE(m_WM_TASKBARCREATED, &CTrafficMonitorDlg::OnTaskBarCreated)
	ON_COMMAND(ID_TRAFFIC_HISTORY, &CTrafficMonitorDlg::OnTrafficHistory)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_OPTIONS, &CTrafficMonitorDlg::OnOptions)
	ON_COMMAND(ID_OPTIONS2, &CTrafficMonitorDlg::OnOptions2)
	ON_MESSAGE(WM_EXITMENULOOP, &CTrafficMonitorDlg::OnExitmenuloop)
	ON_COMMAND(ID_CHANGE_NOTIFY_ICON, &CTrafficMonitorDlg::OnChangeNotifyIcon)
END_MESSAGE_MAP()


void CTrafficMonitorDlg::ShowInfo()
{
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit);

	CString format_str;
	if (theApp.m_main_wnd_data.m_hide_unit && theApp.m_main_wnd_data.m_speed_unit != SpeedUnit::AUTO)
		format_str = _T("%s%s");
	else
		format_str = _T("%s%s/s");
	if (!theApp.m_main_wnd_data.swap_up_down)
	{
		str.Format(format_str, theApp.m_main_wnd_data.disp_str.up.c_str(), out_speed.GetString());
		m_disp_up.SetWindowTextEx(str);
		str.Format(format_str, theApp.m_main_wnd_data.disp_str.down.c_str(), in_speed.GetString());
		m_disp_down.SetWindowTextEx(str);
	}
	else		//�����ϴ�������λ��
	{
		str.Format(format_str, theApp.m_main_wnd_data.disp_str.down.c_str(), in_speed.GetString());
		m_disp_up.SetWindowTextEx(str);
		str.Format(format_str, theApp.m_main_wnd_data.disp_str.up.c_str(), out_speed.GetString());
		m_disp_down.SetWindowTextEx(str);
	}
	str.Format(_T("%s%d%%"), theApp.m_main_wnd_data.disp_str.cpu.c_str(), theApp.m_cpu_usage);
	m_disp_cpu.SetWindowTextEx(str);
	str.Format(_T("%s%d%%"), theApp.m_main_wnd_data.disp_str.memory.c_str(), theApp.m_memory_usage);
	m_disp_memory.SetWindowTextEx(str);
	//����Ҫ��ʾ����Ŀ
	if (m_show_more_info)
	{
		m_disp_up.ShowWindow(m_layout_data.show_up_l ? SW_SHOW : SW_HIDE);
		m_disp_down.ShowWindow(m_layout_data.show_down_l ? SW_SHOW : SW_HIDE);
		m_disp_cpu.ShowWindow(m_layout_data.show_cpu_l ? SW_SHOW : SW_HIDE);
		m_disp_memory.ShowWindow(m_layout_data.show_memory_l ? SW_SHOW : SW_HIDE);
	}
	else
	{
		m_disp_up.ShowWindow(m_layout_data.show_up_s ? SW_SHOW : SW_HIDE);
		m_disp_down.ShowWindow(m_layout_data.show_down_s ? SW_SHOW : SW_HIDE);
		m_disp_cpu.ShowWindow(m_layout_data.show_cpu_s ? SW_SHOW : SW_HIDE);
		m_disp_memory.ShowWindow(m_layout_data.show_memory_s ? SW_SHOW : SW_HIDE);
	}
}

CString CTrafficMonitorDlg::GetMouseTipsInfo()
{
	CString tip_info;
	CString temp;
	temp.Format(_T("������ʹ������: %s\r\n"), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_traffic / 1024)));
	tip_info += temp;
	if (m_show_more_info)
	{
		if (!m_layout_data.show_up_l)		//�����������û����ʾ�ϴ��ٶȣ�������ʾ��Ϣ����ʾ�ϴ��ٶ�
		{
			temp.Format(_T("�ϴ�: %s/s\r\n"), CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit));
			tip_info += temp;
		}
		if (!m_layout_data.show_down_l)
		{
			temp.Format(_T("����: %s/s\r\n"), CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit));
			tip_info += temp;
		}
		if (!m_layout_data.show_cpu_l)
		{
			temp.Format(_T("CPUʹ��: %d%%\r\n"), theApp.m_cpu_usage);
			tip_info += temp;
		}
		if (!m_layout_data.show_memory_l)
		{
			temp.Format(_T("�ڴ�ʹ�ã�%s/%s (%d%%)\r\n"), CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
			tip_info += temp;
		}
		else
		{
			temp.Format(_T("�ڴ�ʹ�ã�%s/%s\r\n"), CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
			tip_info += temp;
		}
	}
	else
	{
		if (!m_layout_data.show_up_s)		//�����������û����ʾ�ϴ��ٶȣ�������ʾ��Ϣ����ʾ�ϴ��ٶ�
		{
			temp.Format(_T("�ϴ�: %s/s\r\n"), CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit));
			tip_info += temp;
		}
		if (!m_layout_data.show_down_s)
		{
			temp.Format(_T("����: %s/s\r\n"), CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data.speed_short_mode, theApp.m_main_wnd_data.m_speed_unit, theApp.m_main_wnd_data.m_hide_unit));
			tip_info += temp;
		}
		if (!m_layout_data.show_cpu_s)
		{
			temp.Format(_T("CPUʹ��: %d%%\r\n"), theApp.m_cpu_usage);
			tip_info += temp;
		}
		if (!m_layout_data.show_memory_s)
		{
			temp.Format(_T("�ڴ�ʹ�ã�%s/%s (%d%%)\r\n"), CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
			tip_info += temp;
		}
		else
		{
			temp.Format(_T("�ڴ�ʹ�ã�%s/%s\r\n"), CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
			tip_info += temp;
		}
	}
	return tip_info;
}

void CTrafficMonitorDlg::SetTransparency()
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, m_transparency * 255 / 100, LWA_ALPHA);  //͸����ȡֵ��ΧΪ0~255
}

void CTrafficMonitorDlg::SetTransparency(int transparency)
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, transparency * 255 / 100, LWA_ALPHA);  //͸����ȡֵ��ΧΪ0~255
}

void CTrafficMonitorDlg::SetAlwaysOnTop()
{
	if (!m_is_foreground_fullscreen || (m_is_foreground_fullscreen && !theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen))
	{
		if (m_always_on_top)
			SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
		else
			SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//ȡ���ö�
	}
}

void CTrafficMonitorDlg::SetMousePenetrate()
{
	if (m_mouse_penetrate)
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//������괩͸
		if (!theApp.m_show_notify_icon)	//��괩͸ʱ�����֪ͨͼ��û����ʾ��������ʾ�����������޷������Ҽ��˵�
		{
			//���֪ͨ��ͼ��
			AddNotifyIcon();
			theApp.m_show_notify_icon = true;
		}
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//ȡ����괩͸
	}
}

void CTrafficMonitorDlg::CheckWindowPos()
{
	CRect rect;
	GetWindowRect(rect);
	if (m_screen_rect.Width() <= rect.Width() || m_screen_rect.Height() <= rect.Height())
		return;
	if (rect.left < m_screen_rect.left)
	{
		rect.MoveToX(m_screen_rect.left);
		MoveWindow(rect);
	}
	if (rect.top < m_screen_rect.top)
	{
		rect.MoveToY(m_screen_rect.top);
		MoveWindow(rect);
	}
	if (rect.right > m_screen_rect.right)
	{
		rect.MoveToX(m_screen_rect.right - rect.Width());
		MoveWindow(rect);
	}
	if (rect.bottom > m_screen_rect.bottom)
	{
		rect.MoveToY(m_screen_rect.bottom - rect.Height());
		MoveWindow(rect);
	}
}

void CTrafficMonitorDlg::GetScreenSize()
{
	//m_screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
	//m_screen_height = GetSystemMetrics(SM_CYFULLSCREEN) + compensition_value;

	::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_screen_rect, 0);   // ��ù�������С
}

void CTrafficMonitorDlg::LoadConfig()
{
	m_transparency = GetPrivateProfileInt(_T("config"), _T("transparency"), 80, theApp.m_config_path.c_str());
	m_always_on_top = (GetPrivateProfileInt(_T("config"), _T("always_on_top"), 1, theApp.m_config_path.c_str()) != 0);
	m_lock_window_pos = (GetPrivateProfileInt(_T("config"), _T("lock_window_pos"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_show_notify_icon = (GetPrivateProfileInt(_T("config"), _T("show_notify_icon"), 1, theApp.m_config_path.c_str()) != 0);
	m_show_more_info = (GetPrivateProfileInt(_T("config"), _T("show_cpu_memory"), 1, theApp.m_config_path.c_str()) != 0);
	m_mouse_penetrate = (GetPrivateProfileInt(_T("config"), _T("mouse_penetrate"), 0, theApp.m_config_path.c_str()) != 0);
	m_show_task_bar_wnd = (GetPrivateProfileInt(_T("config"), _T("show_task_bar_wnd"), 0, theApp.m_config_path.c_str()) != 0);
	m_position_x = GetPrivateProfileInt(_T("config"), _T("position_x"), -1, theApp.m_config_path.c_str());
	m_position_y = GetPrivateProfileInt(_T("config"), _T("position_y"), -1, theApp.m_config_path.c_str());
	m_auto_select = (GetPrivateProfileInt(_T("connection"), _T("auto_select"), 1, theApp.m_config_path.c_str()) != 0);
	theApp.m_main_wnd_data.text_color = GetPrivateProfileInt(_T("config"), _T("text_color"), 16384, theApp.m_config_path.c_str());
	theApp.m_hide_main_window = (GetPrivateProfileInt(_T("config"), _T("hide_main_window"), 0, theApp.m_config_path.c_str()) != 0);
	wchar_t buff[256];
	GetPrivateProfileStringW(L"connection", L"connection_name", L"", buff, 256, theApp.m_config_path.c_str());
	m_connection_name = CCommon::UnicodeToStr(buff);
	wchar_t wbuff[32];
	GetPrivateProfileString(_T("config"), _T("skin_selected"), _T(""), wbuff, 32, theApp.m_config_path.c_str());
	m_skin_name = wbuff;
	if (m_skin_name.substr(0, 8) == L".\\skins\\")		//�����ȡ����Ƥ������ǰ����".\\skins\\"�������ɾ���������ں�ǰһ���汾���ּ����ԣ�
		m_skin_name = m_skin_name.substr(7);
	m_notify_icon_selected = GetPrivateProfileInt(_T("config"), _T("notify_icon_selected"), 0, theApp.m_config_path.c_str());
	theApp.m_main_wnd_data.swap_up_down = (GetPrivateProfileInt(_T("config"), _T("swap_up_down"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen = (GetPrivateProfileInt(_T("config"), _T("hide_main_wnd_when_fullscreen"), 1, theApp.m_config_path.c_str()) != 0);

	GetPrivateProfileString(_T("config"), _T("font_name"), _T("΢���ź�"), theApp.m_main_wnd_data.font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	theApp.m_main_wnd_data.font_size = GetPrivateProfileInt(_T("config"), _T("font_size"), 10, theApp.m_config_path.c_str());

	theApp.m_main_wnd_data.disp_str.up = CCommon::GetIniStringW(L"config", L"up_string", L"�ϴ�: $", theApp.m_config_path.c_str());
	theApp.m_main_wnd_data.disp_str.down = CCommon::GetIniStringW(L"config", L"down_string", L"����: $", theApp.m_config_path.c_str());
	theApp.m_main_wnd_data.disp_str.cpu = CCommon::GetIniStringW(L"config", L"cpu_string", L"CPU: $", theApp.m_config_path.c_str());
	theApp.m_main_wnd_data.disp_str.memory = CCommon::GetIniStringW(L"config", L"memory_string", L"�ڴ�: $", theApp.m_config_path.c_str());

	theApp.m_main_wnd_data.speed_short_mode = (GetPrivateProfileInt(_T("config"), _T("speed_short_mode"), 0, theApp.m_config_path.c_str()) != 0);
	theApp.m_main_wnd_data.m_speed_unit = static_cast<SpeedUnit>(GetPrivateProfileInt(_T("config"), _T("speed_unit"), 0, theApp.m_config_path.c_str()));
	theApp.m_main_wnd_data.m_hide_unit = (GetPrivateProfileInt(_T("config"), _T("hide_unit"), 0, theApp.m_config_path.c_str()) != 0);
}

void CTrafficMonitorDlg::SaveConfig()
{
	//����ǰ�Ȼ�ȡ���ڵ�λ��
	CRect rect;
	GetWindowRect(rect);
	m_position_x = rect.left;
	m_position_y = rect.top;
	if (!CCommon::WritePrivateProfileIntW(L"config", L"transparency", m_transparency, theApp.m_config_path.c_str()))
	{
		if (m_cannot_save_config_warning)
			MessageBox(_T("���棺�޷��������ã������Ƿ������������Ŀ¼��д�����ݵ�Ȩ�ޣ��Թ���Ա������г�����ܻ������⡣"), NULL, MB_ICONWARNING);
		m_cannot_save_config_warning = false;
		return;
	}
	CCommon::WritePrivateProfileIntW(L"config", L"always_on_top", m_always_on_top, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"lock_window_pos", m_lock_window_pos, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_notify_icon", theApp.m_show_notify_icon, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_cpu_memory", m_show_more_info, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"mouse_penetrate", m_mouse_penetrate, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"show_task_bar_wnd", m_show_task_bar_wnd, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"position_x", m_position_x, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"position_y", m_position_y, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"connection", L"auto_select", m_auto_select, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"text_color", theApp.m_main_wnd_data.text_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"hide_main_window", theApp.m_hide_main_window, theApp.m_config_path.c_str());
	m_connection_name = m_connections[m_connection_selected].description;
	WritePrivateProfileStringW(L"connection", L"connection_name", CCommon::StrToUnicode(m_connection_name.c_str()).c_str(), theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("config"), _T("skin_selected"), m_skin_name.c_str(), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"notify_icon_selected", m_notify_icon_selected, theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("config"), _T("font_name"), theApp.m_main_wnd_data.font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"font_size", theApp.m_main_wnd_data.font_size, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileIntW(L"config", L"swap_up_down", theApp.m_main_wnd_data.swap_up_down, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"hide_main_wnd_when_fullscreen", theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen, theApp.m_config_path.c_str());

	CCommon::WriteIniStringW(_T("config"), _T("up_string"), theApp.m_main_wnd_data.disp_str.up, theApp.m_config_path.c_str());
	CCommon::WriteIniStringW(_T("config"), _T("down_string"), theApp.m_main_wnd_data.disp_str.down, theApp.m_config_path.c_str());
	CCommon::WriteIniStringW(_T("config"), _T("cpu_string"), theApp.m_main_wnd_data.disp_str.cpu, theApp.m_config_path.c_str());
	CCommon::WriteIniStringW(_T("config"), _T("memory_string"), theApp.m_main_wnd_data.disp_str.memory, theApp.m_config_path.c_str());

	CCommon::WritePrivateProfileIntW(L"config", L"speed_short_mode", theApp.m_main_wnd_data.speed_short_mode, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"speed_unit", static_cast<int>(theApp.m_main_wnd_data.m_speed_unit), theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"config", L"hide_unit", theApp.m_main_wnd_data.m_hide_unit, theApp.m_config_path.c_str());
}

void CTrafficMonitorDlg::AutoSelect()
{
	unsigned int max_in_out_bytes{};
	unsigned int in_out_bytes;
	//m_connection_selected = m_connections[0].index;
	m_connection_selected = 0;
	//�Զ�ѡ������ʱ�������ѷ��ͺ��ѽ����ֽ���֮�������Ǹ����ӣ�����������Ϊ��ǰ�鿴������
	for (int i{}; i<m_connections.size(); i++)
	{
		if (m_pIfTable->table[m_connections[i].index].dwOperStatus == IF_OPER_STATUS_OPERATIONAL)		//ֻѡ������״̬Ϊ����������
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
	//Ϊm_pIfTable���������С���ڴ�
	free(m_pIfTable);
	m_dwSize = sizeof(MIB_IFTABLE);
	m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);
	int rtn;
	rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	if (rtn == ERROR_INSUFFICIENT_BUFFER)	//�����������ֵΪERROR_INSUFFICIENT_BUFFER��˵��m_pIfTable�Ĵ�С����
	{
		free(m_pIfTable);
		m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);	//���µĴ�С���¿���һ���ڴ�
	}
	//��ȡ��ǰ���е����ӣ������浽m_connections������
	m_connections.clear();
	GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	for (unsigned int i{}; i < m_pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)m_pIfTable->table[i].bDescr;
		if (m_pIfTable->table[i].dwInOctets > 0 || m_pIfTable->table[i].dwOutOctets > 0 || descr == m_connection_name)		//���ҽ��ջ�������������0�����Ӻ��ϴ�ѡ�������
		{
			m_connections.emplace_back(i, descr, m_pIfTable->table[i].dwInOctets, m_pIfTable->table[i].dwOutOctets);
		}
	}
	if (m_connections.empty())
		m_connections.emplace_back(0, string((const char*)m_pIfTable->table[0].bDescr), 0, 0);

	//if (m_connection_selected < 0 || m_connection_selected >= m_connections.size() || m_auto_select)
	//	AutoSelect();
	//ѡ����������
	if (m_auto_select)	//�Զ�ѡ��
	{
		if (m_restart_cnt != -1)	//��m_restart_cnt������-1ʱ�������ǵ�һ�γ�ʼ��ʱ����Ҫ��ʱ5�������³�ʼ������
		{
			KillTimer(DELAY_TIMER);
			SetTimer(DELAY_TIMER, 5000, NULL);
		}
		else
		{
			AutoSelect();
		}
	}
	else		//����������Ϊ�ϴ�ѡ�������
	{
		m_connection_selected = 0;
		for (size_t i{}; i < m_connections.size(); i++)
		{
			if (m_connections[i].description == m_connection_name)
				m_connection_selected = i;
		}
	}
	m_connection_name = m_connections[m_connection_selected].description;

	//�����ѻ�ȡ���������ڲ˵��������Ӧ��Ŀ
	m_menu.DestroyMenu();
	m_menu.LoadMenu(IDR_MENU1); //װ���Ҽ��˵�
	m_select_connection_menu = m_menu.GetSubMenu(0)->GetSubMenu(0);		//���á�ѡ���������Ӳ˵���
	CString connection_descr;
	for (int i{}; i < m_connections.size(); i++)
	{
		connection_descr = CCommon::StrToUnicode(m_connections[i].description.c_str()).c_str();
		m_select_connection_menu->AppendMenu(MF_STRING | MF_ENABLED, ID_SELETE_CONNECTION + i + 1, connection_descr);
	}

	m_restart_cnt++;	//��¼��ʼ������
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
	////���û�������������ö������ڴ�����������ʱ�������������ö�����ȡ���ö����Խ�����������ڵ������ʾ��ʾ�����������������
	//if (!m_always_on_top)
	//	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//�����ö�
	m_tBarDlg = new CTaskBarDlg;
	m_tBarDlg->Create(IDD_TASK_BAR_DIALOG, this);
	m_tBarDlg->ShowWindow(SW_SHOW);
	m_tBarDlg->ShowInfo();
	//if (!m_always_on_top)
	//	SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//ȡ���ö�
}

void CTrafficMonitorDlg::AddNotifyIcon()
{
	if (m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//���֪ͨ��ͼ��
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
	if (m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::DeleteNotifyIcon()
{
	if (m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//ɾ��֪ͨ��ͼ��
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
	if (m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::SaveHistoryTraffic()
{
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
			if (m_history_traffics.size() > 9999) break;		//����ȡ10000�����ʷ��¼
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
		//����ȡ������ʷ�����б����ڴӴ�С����
		std::sort(m_history_traffics.begin(), m_history_traffics.end(), HistoryTraffic::DateGreater);

		//����б�������ͬ���ڵ���Ŀ�������ϲ�
		for (int i{}; i < m_history_traffics.size() - 1; i++)
		{
			if (HistoryTraffic::DateEqual(m_history_traffics[i], m_history_traffics[i + 1]))
			{
				m_history_traffics[i].kBytes += m_history_traffics[i + 1].kBytes;
				m_history_traffics.erase(m_history_traffics.begin() + i + 1);
			}
		}
	}

	//����б��һ����Ŀ�������ǽ��죬�򽫵�һ����Ŀͳ�Ƶ�������Ϊ����ʹ�õ��������������б��ǰ�����һ������Ϊ�������Ŀ
	if (HistoryTraffic::DateEqual(m_history_traffics[0], traffic))
		theApp.m_today_traffic = static_cast<__int64>(m_history_traffics[0].kBytes) * 1024;
	else
		m_history_traffics.push_front(traffic);

}

void CTrafficMonitorDlg::_OnOptions(int tab)
{
	COptionsDlg optionsDlg(tab);
	//��ѡ���������ݴ��ݸ�ѡ�����öԻ���
	optionsDlg.m_tab1_dlg.m_data = theApp.m_main_wnd_data;
	optionsDlg.m_tab2_dlg.m_data = theApp.m_taskbar_data;
	optionsDlg.m_tab3_dlg.m_data = theApp.m_general_data;
	
	if (optionsDlg.DoModal() == IDOK)
	{
		theApp.m_main_wnd_data = optionsDlg.m_tab1_dlg.m_data;
		ApplySettings();
		SaveConfig();

		theApp.m_taskbar_data = optionsDlg.m_tab2_dlg.m_data;
		CTaskBarDlg::SaveConfig();
		if (m_tBarDlg != nullptr)
		{
			m_tBarDlg->ApplySettings();
			//��������������������������ʾ���ı��������������ڿ���Ҫ�仯�����ǹر��ٴ�����������
			CloseTaskBarWnd();
			OpenTaskBarWnd();
		}

		theApp.m_general_data = optionsDlg.m_tab3_dlg.m_data;
		if(optionsDlg.m_tab3_dlg.IsAutoRunModified())
			theApp.SetAutoRun(theApp.m_general_data.auto_run);
		theApp.SaveConfig();
	}
}

void CTrafficMonitorDlg::SetItemPosition()
{
	if (m_show_more_info)
	{
		SetWindowPos(nullptr, 0, 0, m_layout_data.width_l, m_layout_data.height_l, SWP_NOMOVE | SWP_NOZORDER);
		m_disp_up.MoveWindow(m_layout_data.up_x_l, m_layout_data.up_y_l, m_layout_data.up_width_l, m_layout_data.text_height);
		m_disp_down.MoveWindow(m_layout_data.down_x_l, m_layout_data.down_y_l, m_layout_data.down_width_l, m_layout_data.text_height);
		m_disp_cpu.MoveWindow(m_layout_data.cpu_x_l, m_layout_data.cpu_y_l, m_layout_data.cpu_width_l, m_layout_data.text_height);
		m_disp_memory.MoveWindow(m_layout_data.memory_x_l, m_layout_data.memory_y_l, m_layout_data.memory_width_l, m_layout_data.text_height);
	}
	else
	{
		SetWindowPos(nullptr, 0, 0, m_layout_data.width_s, m_layout_data.height_s, SWP_NOMOVE | SWP_NOZORDER);
		m_disp_up.MoveWindow(m_layout_data.up_x_s, m_layout_data.up_y_s, m_layout_data.up_width_s, m_layout_data.text_height);
		m_disp_down.MoveWindow(m_layout_data.down_x_s, m_layout_data.down_y_s, m_layout_data.down_width_s, m_layout_data.text_height);
		m_disp_cpu.MoveWindow(m_layout_data.cpu_x_s, m_layout_data.cpu_y_s, m_layout_data.cpu_width_s, m_layout_data.text_height);
		m_disp_memory.MoveWindow(m_layout_data.memory_x_s, m_layout_data.memory_y_s, m_layout_data.memory_width_s, m_layout_data.text_height);
	}
}

void CTrafficMonitorDlg::GetSkinLayout()
{
	wstring skin_cfg_path{ theApp.m_skin_path + m_skins[m_skin_selected] + L"\\skin.ini" };
	CSkinDlg::GetSkinLayout(skin_cfg_path, m_layout_data);
}

void CTrafficMonitorDlg::ApplySettings()
{
	//Ӧ��������ɫ����
	m_disp_cpu.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_memory.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_up.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_down.SetTextColor(theApp.m_main_wnd_data.text_color);
	//Ӧ����������
	if (m_font.m_hObject)	//���m_font�Ѿ�������һ��������Դ�������ͷ���
		m_font.DeleteObject();
	m_font.CreatePointFont(theApp.m_main_wnd_data.font_size * 10, theApp.m_main_wnd_data.font_name);
	m_disp_cpu.SetFont(&m_font);
	m_disp_memory.SetFont(&m_font);
	m_disp_up.SetFont(&m_font);
	m_disp_down.SetFont(&m_font);
}

// CTrafficMonitorDlg ��Ϣ�������

BOOL CTrafficMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	SetWindowText(_T("TrafficMonitor"));
	//��������������ͼ��
	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	theApp.GetDPI(this);
	//��ȡ��Ļ��С
	//if (theApp.m_is_windows10_fall_creator)
	//	GetScreenSize(theApp.DPI(23));
	//else
		GetScreenSize();

	//���ô���͸����
	SetTransparency();

	IniConnection();	//��ʼ������

	//�������ʱ��������괩͸�����������ڣ�����û����ʾ���������ڣ�����ʾ֪ͨ��ͼ��
	if ((m_mouse_penetrate || theApp.m_hide_main_window) && !m_show_task_bar_wnd)
		theApp.m_show_notify_icon = true;

	//����֪ͨ��ͼ��
	theApp.m_notify_icons[0] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[1] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON2), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[2] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON3), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[3] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

	//����֪ͨ����ͼ��
	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);	//�ýṹ������Ĵ�С
	if (m_notify_icon_selected < 0 || m_notify_icon_selected >= MAX_NOTIFY_ICON)
		m_notify_icon_selected = 0;
	m_ntIcon.hIcon = theApp.m_notify_icons[m_notify_icon_selected];		//����ͼ��
	m_ntIcon.hWnd = this->m_hWnd;				//��������ͼ��֪ͨ��Ϣ�Ĵ��ھ��
#ifdef _DEBUG
	TCHAR atip[128] = _T("��������� (Debug)");			//���ָ��ͼ��ʱ��ʾ����ʾ
#else
	TCHAR atip[128] = _T("���������");
#endif
	wcscpy_s(m_ntIcon.szTip, 128, atip);
	m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON;	//Ӧ�ó��������ϢID��
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;	//ͼ������ԣ����ó�ԱuCallbackMessage��hIcon��szTip��Ч
	if (theApp.m_show_notify_icon)
		::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);	//��ϵͳ֪ͨ�����������ͼ��

	//����������ʷ��¼
	LoadHistoryTraffic();

	//����1000���봥���Ķ�ʱ��
	SetTimer(MAIN_TIMER, 1000, NULL);

	//��ʼ��Ƥ��
	CCommon::GetFiles(theApp.m_skin_path.c_str(), m_skins);
	if (m_skins.empty())
		m_skins.push_back(L"");
	m_skin_selected = 0;
	for (unsigned int i{}; i<m_skins.size(); i++)
	{
		if (m_skins[i] == m_skin_name)
			m_skin_selected = i;
	}

	//���ݵ�ǰѡ���Ƥ����ȡ��������
	GetSkinLayout();

	//��ʼ������λ��
	if (m_position_x != -1 && m_position_y != -1)
		SetWindowPos(nullptr, m_position_x, m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	SetItemPosition();
	CheckWindowPos();

	//���뱳��ͼƬ
	if (m_show_more_info)
		m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_l, m_layout_data.height_l, LR_LOADFROMFILE);
	else
		m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_s, m_layout_data.height_s, LR_LOADFROMFILE);
	SetBackgroundImage(m_back_img);

	//����������ɫ
	m_disp_cpu.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_memory.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_up.SetTextColor(theApp.m_main_wnd_data.text_color);
	m_disp_down.SetTextColor(theApp.m_main_wnd_data.text_color);

	//��������
	m_font.CreatePointFont(theApp.m_main_wnd_data.font_size * 10, theApp.m_main_wnd_data.font_name);
	m_disp_cpu.SetFont(&m_font);
	m_disp_memory.SetFont(&m_font);
	m_disp_up.SetFont(&m_font);
	m_disp_down.SetFont(&m_font);


	////��ȡ���������ļ��е�·��
	//m_start_up_path = CCommon::GetStartUpPath();
	////�жϳ������������ݷ�ʽ�Ƿ����
	//m_auto_run = CCommon::FileExist((m_start_up_path + L"\\TrafficMonitor.lnk").c_str());

	//��ȡ����ʱ��ʱ��
	GetLocalTime(&m_start_time);

	//��ʼ�������ʾ
	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetMaxTipWidth(600);
	m_tool_tips.AddTool(this, _T(""));

	//�����������ʱ���������������ڣ��򴰿ڵ�λ�������Ͻǣ����Ƚ��䲻͸������Ϊ0
	if (theApp.m_hide_main_window || (m_position_x == 0 && m_position_y == 0))
		SetTransparency(0);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CTrafficMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTrafficMonitorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == MAIN_TIMER)
	{
		if (m_first_start)		//���if����ڳ���������1��ִ��
		{
			//�����ô����ö��Ĵ���������������ڽ��
			//���ڳ�ʼ�������п��ܻ���������ö���Ч������
			SetAlwaysOnTop();		//���ô����ö�
			SetMousePenetrate();	//������괩͸
			if (theApp.m_hide_main_window)	//��������������
				ShowWindow(SW_HIDE);
			
			//������������
			if (m_show_task_bar_wnd && m_tBarDlg == nullptr)
				OpenTaskBarWnd();

			//������ڵ�λ��Ϊ(0, 0)�����ڳ�ʼ��ʱMoveWindow������Ч����ʱ���ƶ�һ�δ���
			if (m_position_x == 0 && m_position_y == 0)
			{
				SetWindowPos(nullptr, m_position_x, m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			SetTransparency();				//�������ô��ڲ�͸����

			m_first_start = false;
		}

		////��������2�����ʾ���������ڣ���ֹ��������ʱ������������û�м��ص���Ƕ��������ʧ�ܵ�����
		//if (theApp.WhenStart() && m_timer_cnt == 2)
		//{
		//	if (m_show_task_bar_wnd && m_tBarDlg == nullptr)
		//		OpenTaskBarWnd();
		//}

		if (m_always_on_top && !theApp.m_hide_main_window)
		{
			//ÿ��1���Ӿ��ж�һ��ǰ̨�����Ƿ�ȫ��
			m_is_foreground_fullscreen = CCommon::IsForegroundFullscreen();
			if (theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen)		//��������ȫ��ʱ����������ʱ
			{
				if(m_is_foreground_fullscreen || theApp.m_hide_main_window)
					ShowWindow(SW_HIDE);
				else
					ShowWindow(SW_RESTORE);
			}
			else			//���û������ȫ��ʱ������������������г������ȫ��״̬��������һ�δ����ö�
			{
				static bool last_foreground_fullscreen;
				if (!last_foreground_fullscreen && m_is_foreground_fullscreen)
				{
					SetAlwaysOnTop();
				}
				last_foreground_fullscreen = m_is_foreground_fullscreen;
			}
		}

		if (!m_menu_popuped)
		{
			//�����������������ʱ�������������ִ�С������ö���������괩͸���͡����������ڡ��Ĳ�������ֹ����û����Ч
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

			if (m_timer_cnt % 300 == 299 && !theApp.m_hide_main_window && m_always_on_top)
			{
				SetAlwaysOnTop();		//ÿ5����ִ��һ�����ô����ö�
			}
		}

		if (m_timer_cnt % 15 == 14)		//ÿ��15���ӱ���һ������
			SaveConfig();

		if (m_timer_cnt % 2 == 1)		//ÿ��2���ӻ�ȡһ����Ļ����
		{
			GetScreenSize();
			CheckWindowPos();
		}

		//��ȡ���������ٶ�
		int rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
		m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets;
		m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets;

		//������ͺͽ��յ��ֽ���Ϊ0���ϴη��ͺͽ��յ��ֽ���Ϊ0��ǰ�����Ѹı�ʱ��������Ч
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
		//�������1GB/s��˵�����ܲ������쳣��������Ч
		if (theApp.m_in_speed > 1073741824)
			theApp.m_in_speed = 0;
		if (theApp.m_out_speed > 1073741824)
			theApp.m_out_speed = 0;

		m_connection_change_flag = false;	//������ӷ����仯�ı�־

		m_last_in_bytes = m_in_bytes;
		m_last_out_bytes = m_out_bytes;

		//�����Զ�ѡ��״̬ʱ���������30��û�����٣�������Զ�ѡ������粻�ԣ���ʱִ��һ���Զ�ѡ��
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

		//��⵱ǰ�����Ƿ�ı䣬����Ѹı䣬������ʷ�����б����һ���µ�����
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

		//ͳ�ƽ�����ʹ�õ�����
		theApp.m_today_traffic += (theApp.m_in_speed + theApp.m_out_speed);
		m_history_traffics[0].kBytes = static_cast<unsigned int>(theApp.m_today_traffic / 1024);
		//ÿ��30�뱣��һ��������ʷ��¼
		if (m_timer_cnt % 30 == 10)
			SaveHistoryTraffic();

		char buff[256];
		if (rtn == ERROR_INSUFFICIENT_BUFFER)
		{
			IniConnection();
			sprintf_s(buff, "���ڴ���������Ϣ�Ļ�������С�����������³�ʼ�����ӡ�(�����³�ʼ��%d��)", m_restart_cnt);
			CCommon::WriteLog(buff, theApp.m_log_path.c_str());
		}

		//ͳ�Ƶ�ǰ�ѷ��ͻ��ѽ����ֽ�����Ϊ0�����Ӹ���
		int connection_count{};
		string descr;
		for (unsigned int i{}; i < m_pIfTable->dwNumEntries; i++)
		{
			descr = (const char*)m_pIfTable->table[i].bDescr;
			if (m_pIfTable->table[i].dwInOctets > 0 || m_pIfTable->table[i].dwOutOctets > 0 || descr == m_connection_name)
				connection_count++;
		}
		if (connection_count == 0) connection_count = 1;
		if (connection_count != m_connections.size())	//��������������仯�������³�ʼ������
		{
			sprintf_s(buff, "��⵽�����������仯�������»�ȡ���ӡ���ǰ��������%d��������������%d��(�����³�ʼ��%d��)", m_connections.size(), connection_count, m_restart_cnt + 1);
			IniConnection();
			CCommon::WriteLog(buff, theApp.m_log_path.c_str());
		}
		descr = (const char*)m_pIfTable->table[m_connections[m_connection_selected].index].bDescr;
		if (descr != m_connection_name)
		{
			IniConnection();
			sprintf_s(buff, "���ܳ������쳣����ǰѡ������Ӻ����������Ӳ�һ�£������»�ȡ���ӡ�(�����³�ʼ��%d��)", m_restart_cnt);
			CCommon::WriteLog(buff, theApp.m_log_path.c_str());
		}

		//ֻ�������ں�����������������һ����ʾʱ��ִ������Ĵ���
		if (!theApp.m_hide_main_window || m_show_task_bar_wnd)
		{
				//��ȡCPU������
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
					//���ܵ�ʱ��-����ʱ�䣩/�ܵ�ʱ��=ռ��cpu��ʱ�����ʹ����
					theApp.m_cpu_usage = static_cast<int>(abs((kernel + user - idle) * 100 / (kernel + user)));
				}
				m_preidleTime = idleTime;
				m_prekernelTime = kernelTime;
				m_preuserTime = userTime;
		
				//��ȡ�ڴ�������
				MEMORYSTATUSEX statex;
				statex.dwLength = sizeof(statex);
				GlobalMemoryStatusEx(&statex);
				theApp.m_memory_usage = statex.dwMemoryLoad;
				theApp.m_used_memory = static_cast<int>((statex.ullTotalPhys - statex.ullAvailPhys) / 1024);
				theApp.m_total_memory  = static_cast<int>(statex.ullTotalPhys / 1024);

			ShowInfo();		//ˢ�´�����Ϣ
	
			//���������ʾ
			CString tip_info;
			tip_info = GetMouseTipsInfo();
			m_tool_tips.UpdateTipText(tip_info, this);
			//�������������������ʾ
			if (m_tBarDlg != nullptr)
				m_tBarDlg->UpdateToolTips();

			//ÿ��10���Ӽ��һ���Ƿ����Ƕ��������
			if (m_tBarDlg != nullptr && m_timer_cnt % 10 == 1)
			{
				if (m_tBarDlg->GetCannotInsertToTaskBar() && m_insert_to_taskbar_cnt < MAX_INSERT_TO_TASKBAR_CNT)
				{
					CloseTaskBarWnd();
					OpenTaskBarWnd();
					m_insert_to_taskbar_cnt++;
					if (m_insert_to_taskbar_cnt == MAX_INSERT_TO_TASKBAR_CNT)
					{
						if (m_cannot_intsert_to_task_bar_warning)		//ȷ����ʾ��Ϣֻ����һ��
						{
							MessageBox(_T("���棺����û�гɹ�Ƕ���������������ѱ���ȫ�����ֹ��"), NULL, MB_ICONWARNING);
							m_cannot_intsert_to_task_bar_warning = false;
						}
					}
				}
				if(!m_tBarDlg->GetCannotInsertToTaskBar())
				{
					m_insert_to_taskbar_cnt = 0;
				}
			}
		}

		m_timer_cnt++;
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//���õ������Ҽ������˵�
	CMenu* pContextMenu = m_menu.GetSubMenu(0); //��ȡ��һ�������˵������Ե�һ���˵��������Ӳ˵� 
	CPoint point1;	//����һ������ȷ�����λ�õ�λ��  
	GetCursorPos(&point1);	//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����
	pContextMenu->SetDefaultItem(ID_NETWORK_INFO);
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //��ָ��λ����ʾ�����˵�

	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTrafficMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//��δ��������λ��ʱ����ͨ����������ڲ����϶�����
	if (!m_lock_window_pos)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CTrafficMonitorDlg::OnNetworkInfo()
{
	// TODO: �ڴ���������������
	//�������������顱�Ի���
	CNetworkInfoDlg aDlg(m_pIfTable->table[m_connections[m_connection_selected].index]);
	//��CNetworkInfoDlg�ഫ�������������ѷ��ͺͽ��յ��ֽ���
	aDlg.m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets - m_connections[m_connection_selected].in_bytes;
	aDlg.m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets - m_connections[m_connection_selected].out_bytes;
	aDlg.m_start_time = m_start_time;
	aDlg.DoModal();
	//SetAlwaysOnTop();	//�����ڡ��������顱�Ի�����������ȡ�������ö��������ڶԻ���رպ��������ô����ö�
	if(m_tBarDlg!=nullptr)
		m_tBarDlg->m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);	//�����������������ڵ���ʾ��Ϣ�ö�
}


void CTrafficMonitorDlg::OnAlwaysOnTop()
{
	// TODO: �ڴ���������������
	m_always_on_top = !m_always_on_top;
	SetAlwaysOnTop();
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateAlwaysOnTop(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_always_on_top);
}


void CTrafficMonitorDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialogEx::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ���������������
	m_transparency = 100;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency80()
{
	// TODO: �ڴ���������������
	m_transparency = 80;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency60()
{
	// TODO: �ڴ���������������
	m_transparency = 60;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency40()
{
	// TODO: �ڴ���������������
	m_transparency = 40;
	SetTransparency();
	SaveConfig();
}


void CTrafficMonitorDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_cannot_save_config_warning = true;
	SaveConfig();	//�˳�ǰ�������õ�ini�ļ�
	SaveHistoryTraffic();

	if (m_tBarDlg != nullptr)
		m_tBarDlg->OnCancel();

	CDialogEx::OnClose();
}


BOOL CTrafficMonitorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	UINT uMsg = LOWORD(wParam);
	//ѡ���ˡ�ѡ���������ӡ��Ӳ˵�����Ŀʱ�Ĵ���
	if (uMsg == ID_SELETE_CONNECTION)	//ѡ���ˡ��Զ�ѡ�񡱲˵���
	{
		AutoSelect();
		m_auto_select = true;
		SaveConfig();
		m_connection_change_flag = true;
	}
	if (uMsg > ID_SELETE_CONNECTION && uMsg <= ID_SELETE_CONNECTION + m_connections.size())	//ѡ����һ����������
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

	// TODO: �ڴ˴������Ϣ����������
	m_menu_popuped = true;

	//���á�ѡ�����ӡ��Ӳ˵����и���ѡ���ѡ��״̬
	if (m_auto_select)		//m_auto_selectΪtrueʱΪ�Զ�ѡ��ѡ�в˵��ĵ�1��
		m_select_connection_menu->CheckMenuRadioItem(0, m_connections.size(), 0, MF_BYPOSITION | MF_CHECKED);
	else		//m_auto_selectΪfalseʱ���Զ�ѡ�񣬸���m_connection_selected��ֵѡ���Ӧ����
		m_select_connection_menu->CheckMenuRadioItem(0, m_connections.size(), m_connection_selected + 1, MF_BYPOSITION | MF_CHECKED);

	//���á����ڲ�͸���ȡ��Ӳ˵��¸���ѡ���ѡ��״̬
	switch (m_transparency)
	{
	case 100: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_100, MF_BYCOMMAND | MF_CHECKED); break;
	case 80: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_80, MF_BYCOMMAND | MF_CHECKED); break;
	case 60: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_60, MF_BYCOMMAND | MF_CHECKED); break;
	case 40: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_40, MF_BYCOMMAND | MF_CHECKED); break;
	default: break;
	}

	if(!m_show_task_bar_wnd && (theApp.m_hide_main_window || m_mouse_penetrate))	//���û����ʾ���������ڣ��������������ڻ���������괩͸������á���ʾ֪ͨ��ͼ�ꡱ�˵���
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);
	else
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);

	//pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


BOOL CTrafficMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//���ΰ��س�����ESC���˳�
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
	// TODO: �ڴ���������������
	m_lock_window_pos = !m_lock_window_pos;
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateLockWindowPos(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_lock_window_pos);
}


void CTrafficMonitorDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: �ڴ˴������Ϣ����������
	//ȷ�����ڲ��ᳬ����Ļ��Χ
	CheckWindowPos();
}



afx_msg LRESULT CTrafficMonitorDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	bool dialog_exist{ false };
	const int WIND_NUM{ 6 };
	const CString diloge_title[WIND_NUM]{ _T("���� TrafficMonitor"),_T("����"), _T("��ʷ����ͳ��"), _T("��������"), _T("����Ƥ��"), _T("ѡ������") };
	HWND handle{};
	//���β��ҳ����е�ÿһ���Ի�������ҵ�һ��û�йرյĶԻ���ʱ�����������Ҽ��˵�����ֹ�û��ڴ�ʱ�˳�����
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
			if (dialog_exist)		//�д򿪵ĶԻ���ʱ�����֪ͨ��ͼ��󽫽������õ��Ի���
			{
				::SetForegroundWindow(handle);
			}
			else		//û�д򿪵ĶԻ���ʱ������ʾ������
			{
				ShowWindow(SW_RESTORE);
				theApp.m_hide_main_window = false;
				SetForegroundWindow();
				CheckWindowPos();
				SaveConfig();
			}
		}
	}
	if (lParam == WM_RBUTTONUP && !dialog_exist)
	{
		//��֪ͨ������Ҽ������Ҽ��˵�
		if (m_tBarDlg != nullptr)		//�����ʾ�����������ڣ������һ���֪ͨ��ͼ��󽫽������õ�����������
			m_tBarDlg->SetForegroundWindow();
		else				//���򽫽������õ�������
			SetForegroundWindow();
		CPoint point1;	//����һ������ȷ�����λ�õ�λ��  
		GetCursorPos(&point1);	//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����
		m_menu.GetSubMenu(0)->SetDefaultItem(-1);		//����û��Ĭ�ϲ˵���
		m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //��ָ��λ����ʾ�����˵�

		CheckWindowPos();
	}
	if (lParam == WM_LBUTTONDBLCLK)
	{
		if (dialog_exist)		//�д򿪵ĶԻ���ʱ�����֪ͨ��ͼ��󽫽������õ��Ի���
		{
			::SetForegroundWindow(handle);
		}
		else		//û�д򿪵ĶԻ���ʱ������ʾ������
		{
			ShowWindow(SW_RESTORE);
			theApp.m_hide_main_window = false;
			SetForegroundWindow();
			CheckWindowPos();
			SaveConfig();
		}
	}
	return 0;
}


void CTrafficMonitorDlg::OnShowNotifyIcon()
{
	// TODO: �ڴ���������������
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
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_show_notify_icon);
}


void CTrafficMonitorDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	//�����˳�ʱɾ��֪ͨ��ͼ��
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
	// TODO: �ڴ˴������Ϣ����������
}


void CTrafficMonitorDlg::OnShowCpuMemory()
{
	// TODO: �ڴ���������������
	CRect rect;
	GetWindowRect(rect);
	if (m_show_more_info)
	{
		rect.right = rect.left + m_layout_data.width_s;
		rect.bottom = rect.top + m_layout_data.height_s;
		MoveWindow(rect);
		CheckWindowPos();
		m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, rect.Width(), rect.Height(), LR_LOADFROMFILE);
		m_show_more_info = false;
	}
	else
	{
		rect.right = rect.left + m_layout_data.width_l;
		rect.bottom = rect.top + m_layout_data.height_l;
		MoveWindow(rect);
		CheckWindowPos();
		m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, rect.Width(), rect.Height(), LR_LOADFROMFILE);
		m_show_more_info = true;
	}
	SetBackgroundImage(m_back_img);
	SetItemPosition();
	ShowInfo();
	SaveConfig();
}


//�����������л���ʾCPU���ڴ�������ʱ�Ĵ���
void CTrafficMonitorDlg::OnShowCpuMemory2()
{
	// TODO: �ڴ���������������
	if (m_tBarDlg != nullptr)
	{
		theApp.m_tbar_show_cpu_memory = !theApp.m_tbar_show_cpu_memory;
		//�л���ʾCPU���ڴ�������ʱ��ɾ�����������ڣ���������ʾ
		CloseTaskBarWnd();
		OpenTaskBarWnd();
	}
}


void CTrafficMonitorDlg::OnUpdateShowCpuMemory(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_show_more_info);
}


void CTrafficMonitorDlg::OnMousePenetrate()
{
	// TODO: �ڴ���������������
	m_mouse_penetrate = !m_mouse_penetrate;
	SetMousePenetrate();
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateMousePenetrate(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_mouse_penetrate);
}


void CTrafficMonitorDlg::OnShowTaskBarWnd()
{
	// TODO: �ڴ���������������
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
		//�ر����������ں����û����ʾ֪ͨ��ͼ�꣬��û����ʾ�����ڻ���������괩͸����֪ͨ��ͼ����ʾ����
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
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_show_task_bar_wnd);
}


void CTrafficMonitorDlg::OnAppAbout()
{
	// TODO: �ڴ���������������
	//���������ڡ��Ի���
	CAboutDlg aDlg;
	aDlg.DoModal();
}



//void CTrafficMonitorDlg::OnAutoRunWhenStart()
//{
//	// TODO: �ڴ���������������
//	if (!m_auto_run)
//	{
//		if (MessageBox(_T("�Ƿ�Ҫ�ڡ���ʼ���˵��ġ�������Ŀ¼�´�������Ŀ�ݷ�ʽ��"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
//		{
//			if (CCommon::CreateFileShortcut(m_start_up_path.c_str(), NULL, _T("TrafficMonitor.lnk")))
//			{
//				CString info;
//				info.Format(_T("�Ѿ��ڡ�%s��·���´����˳���Ŀ�ݷ�ʽ��"), m_start_up_path.c_str());
//				MessageBox(info, NULL, MB_ICONINFORMATION);
//				m_auto_run = true;
//			}
//			else
//			{
//				MessageBox(_T("��ݷ�ʽ����ʧ�ܣ�"), NULL, MB_ICONWARNING);
//			}
//		}
//	}
//	else
//	{
//		if (MessageBox(_T("�Ƿ�Ҫ�ڡ���ʼ���˵��ġ�������Ŀ¼��ɾ������Ŀ�ݷ�ʽ��"), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
//		{
//			if (DeleteFile((m_start_up_path + L"\\TrafficMonitor.lnk").c_str()))
//			{
//				MessageBox(_T("��ݷ�ʽɾ���ɹ���"), NULL, MB_ICONINFORMATION);
//				m_auto_run = false;
//			}
//			else
//			{
//				MessageBox(_T("��ݷ�ʽɾ��ʧ�ܣ�"), NULL, MB_ICONWARNING);
//			}
//		}
//	}
//}


//void CTrafficMonitorDlg::OnUpdateAutoRunWhenStart(CCmdUI *pCmdUI)
//{
//	// TODO: �ڴ������������û����洦��������
//	pCmdUI->SetCheck(m_auto_run);
//}


//����Դ����������ʱ�ᴥ������Ϣ
LRESULT CTrafficMonitorDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (m_tBarDlg != nullptr)
	{
		CloseTaskBarWnd();
		if (theApp.m_show_notify_icon)
		{
			//�������֪ͨ��ͼ��
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
	// TODO: �ڴ���������������
	if (!theApp.m_hide_main_window)
	{
		ShowWindow(SW_HIDE);
		theApp.m_hide_main_window = true;
		//���������ں����û����ʾ֪ͨ��ͼ�꣬������ʾ����
		if (!theApp.m_show_notify_icon && !m_show_task_bar_wnd)
		{
			AddNotifyIcon();
			theApp.m_show_notify_icon = true;
		}
	}
	else
	{
		ShowWindow(SW_RESTORE);
		theApp.m_hide_main_window = false;
	}
	SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateHideMainWnd(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_hide_main_window);
}


void CTrafficMonitorDlg::OnChangeSkin()
{
	// TODO: �ڴ���������������
	//CRect rect;
	//GetWindowRect(rect);
	CSkinDlg skinDlg;
	//��ʼ��CSkinDlg���������
	skinDlg.m_skins = m_skins;
	skinDlg.m_skin_selected = m_skin_selected;
	//skinDlg.m_skin_width = rect.Width();
	//skinDlg.m_skin_height_s = m_layout_data.height_s;
	//skinDlg.m_skin_height_l = m_layout_data.height_l;
	skinDlg.m_pFont = &m_font;
	if (skinDlg.DoModal() == IDOK)
	{
		m_skin_selected = skinDlg.m_skin_selected;
		m_skin_name = m_skins[m_skin_selected];
		GetSkinLayout();
		//���뱳��ͼƬ
		if (m_show_more_info)
			m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_l, m_layout_data.height_l, LR_LOADFROMFILE);
		else
			m_back_img = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_s, m_layout_data.height_s, LR_LOADFROMFILE);
		SetBackgroundImage(m_back_img);
		//��ȡƤ����������ɫ��������������ɫ
		theApp.m_main_wnd_data.text_color = skinDlg.GetTextColor();
		m_disp_cpu.SetTextColor(theApp.m_main_wnd_data.text_color);
		m_disp_memory.SetTextColor(theApp.m_main_wnd_data.text_color);
		m_disp_up.SetTextColor(theApp.m_main_wnd_data.text_color);
		m_disp_down.SetTextColor(theApp.m_main_wnd_data.text_color);

		theApp.m_main_wnd_data.disp_str = skinDlg.GetDispStrings();
		SetItemPosition();
		SaveConfig();
	}
}



void CTrafficMonitorDlg::OnTrafficHistory()
{
	// TODO: �ڴ���������������
	CHistoryTrafficDlg historyDlg(m_history_traffics);
	historyDlg.DoModal();
}


void CTrafficMonitorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTrafficMonitorDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	OnNetworkInfo();
	CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTrafficMonitorDlg::OnOptions()
{
	// TODO: �ڴ���������������
	_OnOptions(0);
}


//ͨ�����������ڵ��Ҽ��˵��򿪡�ѡ��Ի���
void CTrafficMonitorDlg::OnOptions2()
{
	// TODO: �ڴ���������������
	_OnOptions(1);
}


afx_msg LRESULT CTrafficMonitorDlg::OnExitmenuloop(WPARAM wParam, LPARAM lParam)
{
	m_menu_popuped = false;
	return 0;
}


void CTrafficMonitorDlg::OnChangeNotifyIcon()
{
	// TODO: �ڴ���������������
	CIconSelectDlg dlg(m_notify_icon_selected);
	if (dlg.DoModal() == IDOK)
	{
		m_notify_icon_selected = dlg.GetIconSelected();
		m_ntIcon.hIcon = theApp.m_notify_icons[m_notify_icon_selected];
		if (theApp.m_show_notify_icon)
		{
			DeleteNotifyIcon();
			AddNotifyIcon();
		}
		SaveConfig();
	}
}
