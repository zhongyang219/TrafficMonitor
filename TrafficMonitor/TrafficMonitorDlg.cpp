
// TrafficMonitorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CTrafficMonitorDlg �Ի���

//��̬��Ա��ʼ��
unsigned int CTrafficMonitorDlg::m_WM_TASKBARCREATED{ ::RegisterWindowMessage(_T("TaskbarCreated")) };	//ע����������������Ϣ

CTrafficMonitorDlg::CTrafficMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TRAFFICMONITOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
	ON_COMMAND(ID_SHOW_MAIN_WND, &CTrafficMonitorDlg::OnShowMainWnd)
	ON_UPDATE_COMMAND_UI(ID_SHOW_MAIN_WND, &CTrafficMonitorDlg::OnUpdateShowMainWnd)
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
	ON_COMMAND(ID_ALOW_OUT_OF_BORDER, &CTrafficMonitorDlg::OnAlowOutOfBorder)
	ON_UPDATE_COMMAND_UI(ID_ALOW_OUT_OF_BORDER, &CTrafficMonitorDlg::OnUpdateAlowOutOfBorder)
	ON_COMMAND(ID_CHECK_UPDATE, &CTrafficMonitorDlg::OnCheckUpdate)
	ON_MESSAGE(WM_TASKBAR_MENU_POPED_UP, &CTrafficMonitorDlg::OnTaskbarMenuPopedUp)
END_MESSAGE_MAP()


void CTrafficMonitorDlg::ShowInfo()
{
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data);

	CString format_str;
	if (theApp.m_main_wnd_data.hide_unit && theApp.m_main_wnd_data.speed_unit != SpeedUnit::AUTO)
		format_str = _T("%s%s");
	else
		format_str = _T("%s%s/s");
	if (!theApp.m_main_wnd_data.swap_up_down)
	{
		str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.up.c_str()), out_speed.GetString());
		m_disp_up.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.up_align_l : m_layout_data.up_align_s));
		str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.down.c_str()), in_speed.GetString());
		m_disp_down.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.down_align_l : m_layout_data.down_align_s));
	}
	else		//�����ϴ�������λ��
	{
		str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.down.c_str()), in_speed.GetString());
		m_disp_up.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.up_align_l : m_layout_data.up_align_s));
		str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.up.c_str()), out_speed.GetString());
		m_disp_down.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.down_align_l : m_layout_data.down_align_s));
	}
	if (theApp.m_main_wnd_data.hide_percent)
		format_str = _T("%s%d");
	else
		format_str = _T("%s%d%%");
	str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.cpu.c_str()), theApp.m_cpu_usage);
	m_disp_cpu.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.cpu_align_l : m_layout_data.cpu_align_s));
	str.Format(format_str, (m_layout_data.no_text ? _T("") : theApp.m_main_wnd_data.disp_str.memory.c_str()), theApp.m_memory_usage);
	m_disp_memory.SetWindowTextEx(str, (theApp.m_cfg_data.m_show_more_info ? m_layout_data.memory_align_l : m_layout_data.memory_align_s));
	//����Ҫ��ʾ����Ŀ
	if (theApp.m_cfg_data.m_show_more_info)
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
	temp.Format(_T("%s: %s (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
		CCommon::KBytesToString(static_cast<unsigned int>((theApp.m_today_up_traffic + theApp.m_today_down_traffic)/ 1024)),
		CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_up_traffic / 1024)),
		CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_down_traffic / 1024))
		);
	tip_info += temp;
	if (theApp.m_cfg_data.m_show_more_info)
	{
		if (!m_layout_data.show_up_l)		//�����������û����ʾ�ϴ��ٶȣ�������ʾ��Ϣ����ʾ�ϴ��ٶ�
		{
			temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
				CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data));
			tip_info += temp;
		}
		if (!m_layout_data.show_down_l)
		{
			temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
				CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data));
			tip_info += temp;
		}
		if (!m_layout_data.show_cpu_l)
		{
			temp.Format(_T("\r\n%s: %d%%"), CCommon::LoadText(IDS_CPU_USAGE), theApp.m_cpu_usage);
			tip_info += temp;
		}
		if (!m_layout_data.show_memory_l)
		{
			temp.Format(_T("\r\n%s: %s/%s (%d%%)"), CCommon::LoadText(IDS_MEMORY_USAGE),
				CCommon::KBytesToString(theApp.m_used_memory),
				CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
			tip_info += temp;
		}
		else
		{
			temp.Format(_T("\r\n%s: %s/%s"), CCommon::LoadText(IDS_MEMORY_USAGE),
				CCommon::KBytesToString(theApp.m_used_memory),
				CCommon::KBytesToString(theApp.m_total_memory));
			tip_info += temp;
		}
	}
	else
	{
		if (!m_layout_data.show_up_s)		//�����������û����ʾ�ϴ��ٶȣ�������ʾ��Ϣ����ʾ�ϴ��ٶ�
		{
			temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
				CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data));
			tip_info += temp;
		}
		if (!m_layout_data.show_down_s)
		{
			temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
				CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data));
			tip_info += temp;
		}
		if (!m_layout_data.show_cpu_s)
		{
			temp.Format(_T("\r\n%s: %d%%"), CCommon::LoadText(IDS_CPU_USAGE), theApp.m_cpu_usage);
			tip_info += temp;
		}
		if (!m_layout_data.show_memory_s)
		{
			temp.Format(_T("\r\n%s: %s/%s (%d%%)"), CCommon::LoadText(IDS_MEMORY_USAGE),
				CCommon::KBytesToString(theApp.m_used_memory),
				CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
			tip_info += temp;
		}
		else
		{
			temp.Format(_T("\r\n%s: %s/%s"), CCommon::LoadText(IDS_MEMORY_USAGE),
				CCommon::KBytesToString(theApp.m_used_memory),
				CCommon::KBytesToString(theApp.m_total_memory));
			tip_info += temp;
		}
	}
	return tip_info;
}

void CTrafficMonitorDlg::SetTransparency()
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, theApp.m_cfg_data.m_transparency * 255 / 100, LWA_ALPHA);  //͸����ȡֵ��ΧΪ0~255
}

void CTrafficMonitorDlg::SetTransparency(int transparency)
{
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(0, transparency * 255 / 100, LWA_ALPHA);  //͸����ȡֵ��ΧΪ0~255
}

void CTrafficMonitorDlg::SetAlwaysOnTop()
{
	//if (!m_is_foreground_fullscreen || (m_is_foreground_fullscreen && !theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen))
	//{
	//	if (m_always_on_top)
	//		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
	//	else
	//		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//ȡ���ö�
	//}
	if (theApp.m_cfg_data.m_hide_main_window)
		return;
	else if (theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen && m_is_foreground_fullscreen)		//�������г���ȫ��ʱ�������������г�����ȫ������ʱ����ִ���ö�����
		return;

	if (theApp.m_cfg_data.m_always_on_top)
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
	else
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//ȡ���ö�

}

void CTrafficMonitorDlg::SetMousePenetrate()
{
	if (theApp.m_cfg_data.m_mouse_penetrate)
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT);		//������괩͸
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & (~WS_EX_TRANSPARENT));		//ȡ����괩͸
	}
}

void CTrafficMonitorDlg::CheckWindowPos()
{
	if (!theApp.m_cfg_data.m_alow_out_of_border)
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
}

void CTrafficMonitorDlg::GetScreenSize()
{
	//m_screen_width = GetSystemMetrics(SM_CXFULLSCREEN);
	//m_screen_height = GetSystemMetrics(SM_CYFULLSCREEN) + compensition_value;

	::SystemParametersInfo(SPI_GETWORKAREA, 0, &m_screen_rect, 0);   // ��ù�������С
}


void CTrafficMonitorDlg::AutoSelect()
{
	unsigned int max_in_out_bytes{};
	unsigned int in_out_bytes;
	//m_connection_selected = m_connections[0].index;
	m_connection_selected = 0;
	//�Զ�ѡ������ʱ�������ѷ��ͺ��ѽ����ֽ���֮�������Ǹ����ӣ�����������Ϊ��ǰ�鿴������
	for (size_t i{}; i<m_connections.size(); i++)
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
	theApp.m_cfg_data.m_connection_name = m_connections[m_connection_selected].description_2;
	m_connection_change_flag = true;
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
	GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	if (!theApp.m_general_data.show_all_interface)
	{
		CAdapterCommon::GetAdapterInfo(m_connections);
		CAdapterCommon::GetIfTableInfo(m_connections, m_pIfTable);
	}
	else
	{
		CAdapterCommon::GetAllIfTableInfo(m_connections, m_pIfTable);
	}

	//����������ˡ���ʾ�����������ӡ�ʱ�����ˡ�ѡ��ȫ���������Ϊ���Զ�ѡ��
	if (theApp.m_general_data.show_all_interface && theApp.m_cfg_data.m_select_all)
	{
		theApp.m_cfg_data.m_select_all = false;
		theApp.m_cfg_data.m_auto_select = true;
	}

	//д�������־
	if (theApp.m_debug_log)
	{
		CString log_str;
		log_str += _T("���ڳ�ʼ����������...\n");
		log_str += _T("�����б�\n");
		for (size_t i{}; i < m_connections.size(); i++)
		{
			log_str += m_connections[i].description.c_str();
			log_str += _T(", ");
			log_str += CCommon::IntToString(m_connections[i].index);
			log_str += _T("\n");
		}
		log_str += _T("IfTable:\n");
		for (size_t i{}; i < m_pIfTable->dwNumEntries; i++)
		{
			log_str += CCommon::IntToString(i);
			log_str += _T(" ");
			log_str += (const char*)m_pIfTable->table[i].bDescr;
			log_str += _T("\n");
		}
		CCommon::WriteLog(log_str, (theApp.m_config_dir + L".\\connections.log").c_str());
	}

	//if (m_connection_selected < 0 || m_connection_selected >= m_connections.size() || m_auto_select)
	//	AutoSelect();
	//ѡ����������
	if (theApp.m_cfg_data.m_auto_select)	//�Զ�ѡ��
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
			if (m_connections[i].description_2 == theApp.m_cfg_data.m_connection_name)
				m_connection_selected = i;
		}
	}
	if (m_connection_selected < 0 || m_connection_selected >= m_connections.size())
		m_connection_selected = 0;
	theApp.m_cfg_data.m_connection_name = m_connections[m_connection_selected].description_2;

	//�����ѻ�ȡ���������ڲ˵��������Ӧ��Ŀ
	m_menu.DestroyMenu();
	m_menu.LoadMenu(IDR_MENU1); //װ���Ҽ��˵�
	CMenu* select_connection_menu = m_menu.GetSubMenu(0)->GetSubMenu(0);		//���á�ѡ���������ӡ��Ӳ˵���
	IniConnectionMenu(select_connection_menu);		//��ѡ���������Ӳ˵��������Ŀ

	IniTaskBarConnectionMenu();		//��ʼ�������������еġ�ѡ���������ӡ��Ӳ˵���

	m_restart_cnt++;	//��¼��ʼ������
	m_connection_change_flag = true;
}

void CTrafficMonitorDlg::IniConnectionMenu(CMenu * pMenu)
{
	CString connection_descr;
	for (size_t i{}; i < m_connections.size(); i++)
	{
		connection_descr = CCommon::StrToUnicode(m_connections[i].description.c_str()).c_str();
		pMenu->AppendMenu(MF_STRING | MF_ENABLED, ID_SELECT_ALL_CONNECTION + i + 1, connection_descr);
	}
}

void CTrafficMonitorDlg::IniTaskBarConnectionMenu()
{
	//��ʼ�������������еġ�ѡ���������ӡ��Ӳ˵���
	if (m_tBarDlg != nullptr)
	{
		m_tBarDlg->m_menu.DestroyMenu();
		m_tBarDlg->m_menu.LoadMenu(IDR_TASK_BAR_MENU);
		CMenu* select_connection_menu = m_tBarDlg->m_menu.GetSubMenu(0)->GetSubMenu(0);		//���á�ѡ���������ӡ��Ӳ˵���
		IniConnectionMenu(select_connection_menu);		//��ѡ���������Ӳ˵��������Ŀ
	}
}

void CTrafficMonitorDlg::SetConnectionMenuState(CMenu * pMenu)
{
	if (theApp.m_cfg_data.m_select_all)
		pMenu->CheckMenuRadioItem(0, m_connections.size() + 1, 1, MF_BYPOSITION | MF_CHECKED);
	else if (theApp.m_cfg_data.m_auto_select)		//m_auto_selectΪtrueʱΪ�Զ�ѡ��ѡ�в˵��ĵ�1��
		pMenu->CheckMenuRadioItem(0, m_connections.size() + 1, 0, MF_BYPOSITION | MF_CHECKED);
	else		//m_auto_selectΪfalseʱ���Զ�ѡ�񣬸���m_connection_selected��ֵѡ���Ӧ����
		pMenu->CheckMenuRadioItem(0, m_connections.size() + 1, m_connection_selected + 2, MF_BYPOSITION | MF_CHECKED);
}

void CTrafficMonitorDlg::CloseTaskBarWnd()
{
	if (m_tBarDlg != nullptr)
	{
		m_tBarDlg->OnCancel();
		delete m_tBarDlg;
		m_tBarDlg = nullptr;
	}
}

void CTrafficMonitorDlg::OpenTaskBarWnd()
{
	m_tBarDlg = new CTaskBarDlg;
	m_tBarDlg->Create(IDD_TASK_BAR_DIALOG, this);
	m_tBarDlg->ShowWindow(SW_SHOW);
	//m_tBarDlg->ShowInfo();
	IniTaskBarConnectionMenu();
}

void CTrafficMonitorDlg::AddNotifyIcon()
{
	if (theApp.m_cfg_data.m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//���֪ͨ��ͼ��
	::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
	if (theApp.m_cfg_data.m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::DeleteNotifyIcon()
{
	if (theApp.m_cfg_data.m_show_task_bar_wnd)
		CloseTaskBarWnd();
	//ɾ��֪ͨ��ͼ��
	::Shell_NotifyIcon(NIM_DELETE, &m_ntIcon);
	if (theApp.m_cfg_data.m_show_task_bar_wnd)
		OpenTaskBarWnd();
}

void CTrafficMonitorDlg::ShowNotifyTip(const wchar_t * title, const wchar_t * message)
{
	//Ҫ��ʾ֪ͨ����ʾ�������Ƚ�֪ͨ��ͼ����ʾ����
	if (!theApp.m_cfg_data.m_show_notify_icon)
	{
		//���֪ͨ��ͼ��
		AddNotifyIcon();
		theApp.m_cfg_data.m_show_notify_icon = true;
	}
	//��ʾ֪ͨ��ʾ
	m_ntIcon.uFlags |= NIF_INFO;
	//wcscpy_s(m_ntIcon.szInfo, message ? message : _T(""));
	//wcscpy_s(m_ntIcon.szInfoTitle, title ? title : _T(""));
	CCommon::WStringCopy(m_ntIcon.szInfo, 256, message);
	CCommon::WStringCopy(m_ntIcon.szInfoTitle, 64, title);
	::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon);
	m_ntIcon.uFlags &= ~NIF_INFO;
}

void CTrafficMonitorDlg::UpdateNotifyIconTip()
{
	CString strTip;			//���ָ��ͼ��ʱ��ʾ����ʾ
#ifdef _DEBUG
	strTip = CCommon::LoadText(IDS_TRAFFICMONITOR, _T(" (Debug)"));
#else
	strTip = CCommon::LoadText(IDS_TRAFFICMONITOR);
#endif

	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed);

	strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%>/s"), { CCommon::LoadText(IDS_UPLOAD), out_speed });
	strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%>/s"), { CCommon::LoadText(IDS_DOWNLOAD), in_speed });
	strTip += CCommon::StringFormat(_T("\r\nCPU: <%1%>%"), { theApp.m_cpu_usage });
	strTip += CCommon::StringFormat(_T("\r\n<%1%>: <%2%>%"), { CCommon::LoadText(IDS_MEMORY), theApp.m_memory_usage });

	CCommon::WStringCopy(m_ntIcon.szTip, 128, strTip);
	::Shell_NotifyIcon(NIM_MODIFY, &m_ntIcon);

}

void CTrafficMonitorDlg::SaveHistoryTraffic()
{
	ofstream file{ theApp.m_history_traffic_path };
	for (const auto& history_traffic : m_history_traffics)
	{
		char buff[64];
		if (history_traffic.mixed)
			sprintf_s(buff, "%.4d/%.2d/%.2d %u", history_traffic.year, history_traffic.month, history_traffic.day, history_traffic.down_kBytes);
		else
			sprintf_s(buff, "%.4d/%.2d/%.2d %u/%u", history_traffic.year, history_traffic.month, history_traffic.day, history_traffic.up_kBytes, history_traffic.down_kBytes);
		file << buff << std::endl;
	}
	file.close();
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

			int index = current_line.find(L'/', 11);
			traffic.mixed = (index == wstring::npos);
			if (traffic.mixed)
			{
				temp = current_line.substr(11);
				traffic.down_kBytes = atoi(temp.c_str());
				traffic.up_kBytes = 0;
			}
			else
			{
				temp = current_line.substr(11, index - 11);
				traffic.up_kBytes = atoi(temp.c_str());
				temp = current_line.substr(index + 1);
				traffic.down_kBytes = atoi(temp.c_str());
			}
			if (traffic.year > 0 && traffic.month > 0 && traffic.day > 0 && traffic.kBytes() > 0)
				m_history_traffics.push_back(traffic);
		}
	}

	SYSTEMTIME current_time;
	GetLocalTime(&current_time);
	traffic.year = current_time.wYear;
	traffic.month = current_time.wMonth;
	traffic.day = current_time.wDay;
	traffic.up_kBytes = 0;
	traffic.down_kBytes = 0;
	traffic.mixed = false;

	if (m_history_traffics.empty())
	{
		m_history_traffics.push_front(traffic);
	}

	if (m_history_traffics.size() >= 2)
	{
		//����ȡ������ʷ�����б����ڴӴ�С����
		std::sort(m_history_traffics.begin(), m_history_traffics.end(), HistoryTraffic::DateGreater);

		//����б�������ͬ���ڵ���Ŀ�������ϲ�
		for (int i{}; i < static_cast<int>(m_history_traffics.size() - 1); i++)
		{
			if (HistoryTraffic::DateEqual(m_history_traffics[i], m_history_traffics[i + 1]))
			{
				m_history_traffics[i].up_kBytes += m_history_traffics[i + 1].up_kBytes;
				m_history_traffics[i].down_kBytes += m_history_traffics[i + 1].down_kBytes;
				m_history_traffics.erase(m_history_traffics.begin() + i + 1);
			}
		}
	}

	//����б��һ����Ŀ�������ǽ��죬�򽫵�һ����Ŀͳ�Ƶ�������Ϊ����ʹ�õ��������������б��ǰ�����һ������Ϊ�������Ŀ
	if (HistoryTraffic::DateEqual(m_history_traffics[0], traffic))
	{
		theApp.m_today_up_traffic = static_cast<__int64>(m_history_traffics[0].up_kBytes) * 1024;
		theApp.m_today_down_traffic = static_cast<__int64>(m_history_traffics[0].down_kBytes) * 1024;
		m_history_traffics[0].mixed = false;
	}
	else
	{
		m_history_traffics.push_front(traffic);
	}

}

void CTrafficMonitorDlg::_OnOptions(int tab)
{
	COptionsDlg optionsDlg(tab);
	//��ѡ���������ݴ��ݸ�ѡ�����öԻ���
	optionsDlg.m_tab1_dlg.m_data = theApp.m_main_wnd_data;
	optionsDlg.m_tab2_dlg.m_data = theApp.m_taskbar_data;
	optionsDlg.m_tab3_dlg.m_data = theApp.m_general_data;
	optionsDlg.m_tab1_dlg.m_text_disable = m_layout_data.no_text;

	if (optionsDlg.DoModal() == IDOK)
	{
		theApp.m_main_wnd_data = optionsDlg.m_tab1_dlg.m_data;
		theApp.m_taskbar_data = optionsDlg.m_tab2_dlg.m_data;
		theApp.m_general_data = optionsDlg.m_tab3_dlg.m_data;

		ApplySettings();
		theApp.SaveConfig();
		theApp.SaveGlobalConfig();

		//CTaskBarDlg::SaveConfig();
		if (m_tBarDlg != nullptr)
		{
			m_tBarDlg->ApplySettings();
			//��������������������������ʾ���ı��������������ڿ���Ҫ�仯�����ǹر��ٴ�����������
			CloseTaskBarWnd();
			OpenTaskBarWnd();
		}

		if(optionsDlg.m_tab3_dlg.IsAutoRunModified())
			theApp.SetAutoRun(theApp.m_general_data.auto_run);

		if (optionsDlg.m_tab3_dlg.IsShowAllInterfaceModified())
			IniConnection();

		theApp.SaveConfig();
	}
}

void CTrafficMonitorDlg::SetItemPosition()
{
	if (theApp.m_cfg_data.m_show_more_info)
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
	CSkinDlg::LoadSkinLayout(skin_cfg_path, m_layout_data);
	if (m_layout_data.no_text)		//���Ƥ�����ֲ���ʾ�ı������������ϴ������ص�λ�ã���Ϊ�ϴ������ص�λ���Ѿ��̶���Ƥ������
		theApp.m_main_wnd_data.swap_up_down = false;
}

void CTrafficMonitorDlg::LoadBackGroundImage()
{
	m_back_img.Destroy();
	CImage img_tmp;
	CSize image_size;
	if (theApp.m_cfg_data.m_show_more_info)
	{
		img_tmp.Load((theApp.m_skin_path + m_skins[m_skin_selected] + BACKGROUND_IMAGE_L).c_str());
		image_size.SetSize(m_layout_data.width_l, m_layout_data.height_l);
	}
	else
	{
		img_tmp.Load((theApp.m_skin_path + m_skins[m_skin_selected] + BACKGROUND_IMAGE_S).c_str());
		image_size.SetSize(m_layout_data.width_s, m_layout_data.height_s);
	}
	CDrawCommon::BitmapStretch(&img_tmp, &m_back_img, image_size);		//����ͼƬ
	SetBackgroundImage(m_back_img);
	img_tmp.Destroy();

	//������������
	CImage img_mask;
	//��������ͼƬ
	if (theApp.m_cfg_data.m_show_more_info)
		img_tmp.Load((theApp.m_skin_path + m_skins[m_skin_selected] + BACKGROUND_MASK_L).c_str());
	else
		img_tmp.Load((theApp.m_skin_path + m_skins[m_skin_selected] + BACKGROUND_MASK_S).c_str());
	CRgn wnd_rgn;
	if (!img_tmp.IsNull())
	{
		CDrawCommon::BitmapStretch(&img_tmp, &img_mask, image_size);		//��������ͼƬ
		CBitmap bitmap;
		bitmap.Attach(img_mask);
		CDrawCommon::GetRegionFromImage(wnd_rgn, bitmap, 128);		//������ͼƬ��ô��ڵ�����
		bitmap.Detach();
	}
	else
	{
		wnd_rgn.CreateRectRgnIndirect(CRect(CPoint(0, 0), image_size));		//��������ͼƬʧ�ܣ���ʹ�ô��ڴ�С��Ϊ��������
	}
	//�����ȡ���Ĵ�������Ϊ��
	CRgn empty_rgn;
	empty_rgn.CreateRectRgnIndirect(CRect{});	//����һ���յ�CRgn����
	if(wnd_rgn.EqualRgn(&empty_rgn))
		wnd_rgn.SetRectRgn(CRect(CPoint(0, 0), image_size));	//�����ȡ���Ĵ�������Ϊ�գ���ʹ�ô��ڴ�С��Ϊ��������
	SetWindowRgn(wnd_rgn, TRUE);		//���ô�������
}

void CTrafficMonitorDlg::SetTextColor()
{
	int text_colors[MAIN_WND_COLOR_NUM];
	if (theApp.m_main_wnd_data.specify_each_item_color)
	{
		for (int i{}; i < MAIN_WND_COLOR_NUM; i++)
			text_colors[i] = theApp.m_main_wnd_data.text_colors[i];
	}
	else
	{
		for (int i{}; i < MAIN_WND_COLOR_NUM; i++)
			text_colors[i] = theApp.m_main_wnd_data.text_colors[0];
	}

	m_disp_up.SetTextColor(text_colors[0]);
	m_disp_down.SetTextColor(text_colors[1]);
	m_disp_cpu.SetTextColor(text_colors[2]);
	m_disp_memory.SetTextColor(text_colors[3]);
}

void CTrafficMonitorDlg::SetTextFont()
{
	if (m_font.m_hObject)	//���m_font�Ѿ�������һ��������Դ�������ͷ���
		m_font.DeleteObject();
	m_font.CreateFont(
		FONTSIZE_TO_LFHEIGHT(theApp.m_main_wnd_data.font.size), // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		(theApp.m_main_wnd_data.font.bold ? FW_BOLD : FW_NORMAL), // nWeight
		theApp.m_main_wnd_data.font.italic, // bItalic
		theApp.m_main_wnd_data.font.underline, // bUnderline
		theApp.m_main_wnd_data.font.strike_out, // cStrikeOut
		DEFAULT_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		theApp.m_main_wnd_data.font.name);
	m_disp_cpu.SetFont(&m_font);
	m_disp_memory.SetFont(&m_font);
	m_disp_up.SetFont(&m_font);
	m_disp_down.SetFont(&m_font);
}

void CTrafficMonitorDlg::ApplySettings()
{
	//Ӧ��������ɫ����
	SetTextColor();
	//Ӧ����������
	SetTextFont();
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
	GetScreenSize();

	//���ô���͸����
	SetTransparency();

	IniConnection();	//��ʼ������

	//�������ʱ��������괩͸�����������ڣ�����û����ʾ���������ڣ�����ʾ֪ͨ��ͼ��
	if ((theApp.m_cfg_data.m_mouse_penetrate || theApp.m_cfg_data.m_hide_main_window) && !theApp.m_cfg_data.m_show_task_bar_wnd)
		theApp.m_cfg_data.m_show_notify_icon = true;

	//����֪ͨ��ͼ��
	theApp.m_notify_icons[0] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[1] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON2), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[2] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON3), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[3] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
	theApp.m_notify_icons[4] = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_NOFITY_ICON4), IMAGE_ICON, theApp.DPI(16), theApp.DPI(16), LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);

	//����֪ͨ����ͼ��
	m_ntIcon.cbSize = sizeof(NOTIFYICONDATA);	//�ýṹ������Ĵ�С
	if (theApp.m_cfg_data.m_notify_icon_selected < 0 || theApp.m_cfg_data.m_notify_icon_selected >= MAX_NOTIFY_ICON)
		theApp.m_cfg_data.m_notify_icon_selected = 0;
	m_ntIcon.hIcon = theApp.m_notify_icons[theApp.m_cfg_data.m_notify_icon_selected];		//����ͼ��
	m_ntIcon.hWnd = this->m_hWnd;				//��������ͼ��֪ͨ��Ϣ�Ĵ��ھ��
	CString atip;			//���ָ��ͼ��ʱ��ʾ����ʾ
#ifdef _DEBUG
	atip = CCommon::LoadText(IDS_TRAFFICMONITOR, _T(" (Debug)"));
#else
	atip = CCommon::LoadText(IDS_TRAFFICMONITOR);
#endif
	//wcscpy_s(m_ntIcon.szTip, 128, strTip);
	CCommon::WStringCopy(m_ntIcon.szTip, 128, atip.GetString());
	m_ntIcon.uCallbackMessage = MY_WM_NOTIFYICON;	//Ӧ�ó��������ϢID��
	m_ntIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;	//ͼ������ԣ����ó�ԱuCallbackMessage��hIcon��szTip��Ч
	if (theApp.m_cfg_data.m_show_notify_icon)
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
		if (m_skins[i] == theApp.m_cfg_data.m_skin_name)
			m_skin_selected = i;
	}

	//���ݵ�ǰѡ���Ƥ����ȡ��������
	GetSkinLayout();

	//��ʼ������λ��
	SetItemPosition();
	if (theApp.m_cfg_data.m_position_x != -1 && theApp.m_cfg_data.m_position_y != -1)
		SetWindowPos(nullptr, theApp.m_cfg_data.m_position_x, theApp.m_cfg_data.m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
	CheckWindowPos();

	//���뱳��ͼƬ
	LoadBackGroundImage();

	//��������
	SetTextFont();

	//����������ɫ
	SetTextColor();

	//��ȡ����ʱ��ʱ��
	GetLocalTime(&m_start_time);

	//��ʼ�������ʾ
	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetMaxTipWidth(600);
	m_tool_tips.AddTool(this, _T(""));

	//�����������ʱ���������������ڣ��򴰿ڵ�λ�������Ͻǣ����Ƚ��䲻͸������Ϊ0
	if (theApp.m_cfg_data.m_hide_main_window || (theApp.m_cfg_data.m_position_x == 0 && theApp.m_cfg_data.m_position_y == 0))
		SetTransparency(0);

	SetTimer(TASKBAR_TIMER, 100, NULL);

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
			if (theApp.m_cfg_data.m_hide_main_window)	//��������������
				ShowWindow(SW_HIDE);
			
			//������������
			if (theApp.m_cfg_data.m_show_task_bar_wnd && m_tBarDlg == nullptr)
				OpenTaskBarWnd();

			//������ڵ�λ��Ϊ(0, 0)�����ڳ�ʼ��ʱMoveWindow������Ч����ʱ���ƶ�һ�δ���
			if (theApp.m_cfg_data.m_position_x == 0 && theApp.m_cfg_data.m_position_y == 0)
			{
				SetWindowPos(nullptr, theApp.m_cfg_data.m_position_x, theApp.m_cfg_data.m_position_y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
			}
			SetTransparency();				//�������ô��ڲ�͸����

			m_first_start = false;
		}

		if (theApp.m_cfg_data.m_always_on_top && !theApp.m_cfg_data.m_hide_main_window)
		{
			//ÿ��1���Ӿ��ж�һ��ǰ̨�����Ƿ�ȫ��
			m_is_foreground_fullscreen = CCommon::IsForegroundFullscreen();
			if (theApp.m_main_wnd_data.hide_main_wnd_when_fullscreen)		//��������ȫ��ʱ����������ʱ
			{
				if(m_is_foreground_fullscreen || theApp.m_cfg_data.m_hide_main_window)
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
				if (!theApp.m_cfg_data.m_hide_main_window)
				{
					SetAlwaysOnTop();
					SetMousePenetrate();
				}
				else
				{
					ShowWindow(SW_HIDE);
				}
			}

			if (m_timer_cnt % 300 == 299 && !theApp.m_cfg_data.m_hide_main_window && theApp.m_cfg_data.m_always_on_top)
			{
				SetAlwaysOnTop();		//ÿ5����ִ��һ�����ô����ö�
			}
		}

		if (m_timer_cnt % 30 == 26)		//ÿ��30���Ӽ��һ�δ���λ�ã�������λ�÷����仯ʱ��������
		{
			static int last_pos_x{ -1 }, last_pos_y{ -1 };
			if (last_pos_x != theApp.m_cfg_data.m_position_x || last_pos_y != theApp.m_cfg_data.m_position_y)
			{
				theApp.SaveConfig();
				last_pos_x = theApp.m_cfg_data.m_position_x;
				last_pos_y = theApp.m_cfg_data.m_position_y;
			}
		}

		if (m_timer_cnt % 2 == 1)		//ÿ��2���ӻ�ȡһ����Ļ����
		{
			GetScreenSize();
			CheckWindowPos();
		}

		//��ȡ���������ٶ�
		int rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
		if (!theApp.m_cfg_data.m_select_all)		//��ȡ��ǰѡ�����ӵ�����
		{
			m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets;
			m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets;
		}
		else		//��ȡȫ�����ӵ�����
		{
			m_in_bytes = 0;
			m_out_bytes = 0;
			for (size_t i{}; i<m_connections.size(); i++)
			{
				//if (i > 0 && m_pIfTable->table[m_connections[i].index].dwInOctets == m_pIfTable->table[m_connections[i - 1].index].dwInOctets
				//	&& m_pIfTable->table[m_connections[i].index].dwOutOctets == m_pIfTable->table[m_connections[i - 1].index].dwOutOctets)
				//	continue;		//�����б��п��ܻ�����ͬ�����ӣ�ͳ���������ӵ�����ʱ�����Ե��ѷ��ͺ��ѽ����ֽ�����ȫ��ͬ������
				m_in_bytes += m_pIfTable->table[m_connections[i].index].dwInOctets;
				m_out_bytes += m_pIfTable->table[m_connections[i].index].dwOutOctets;
			}
		}

		//������ͺͽ��յ��ֽ���Ϊ0���ϴη��ͺͽ��յ��ֽ���Ϊ0��ǰ�����Ѹı�ʱ��������Ч
		if ((m_in_bytes == 0 && m_out_bytes == 0) || (m_last_in_bytes == 0 && m_last_out_bytes == 0) || m_connection_change_flag)
		{
			theApp.m_in_speed = 0;
			theApp.m_out_speed = 0;
		}
		else
		{
			theApp.m_in_speed = static_cast<unsigned int>(m_in_bytes - m_last_in_bytes);
			theApp.m_out_speed = static_cast<unsigned int>(m_out_bytes - m_last_out_bytes);
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
		if (theApp.m_cfg_data.m_auto_select)
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
			traffic.mixed = false;
			m_history_traffics.push_front(traffic);
			theApp.m_today_up_traffic = 0;
			theApp.m_today_down_traffic = 0;
		}

		//ͳ�ƽ�����ʹ�õ�����
		theApp.m_today_up_traffic += theApp.m_out_speed;
		theApp.m_today_down_traffic += theApp.m_in_speed;
		m_history_traffics[0].up_kBytes = static_cast<unsigned int>(theApp.m_today_up_traffic / 1024);
		m_history_traffics[0].down_kBytes = static_cast<unsigned int>(theApp.m_today_down_traffic / 1024);
		//ÿ��30�뱣��һ��������ʷ��¼
		if (m_timer_cnt % 30 == 10)
		{
			static unsigned int last_today_kbytes;
			if (m_history_traffics[0].kBytes() - last_today_kbytes >= 100u)	//ֻ�е������仯����100KBʱ�ű�����ʷ������¼����ֹ����д�����Ƶ��
			{
				SaveHistoryTraffic();
				last_today_kbytes = m_history_traffics[0].kBytes();
			}
		}

		if (rtn == ERROR_INSUFFICIENT_BUFFER)
		{
			IniConnection();
			CString info;
			info.LoadString(IDS_INSUFFICIENT_BUFFER);
			info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_restart_cnt));
			CCommon::WriteLog(info, theApp.m_log_path.c_str());
		}

		
		if (m_timer_cnt % 3 == 2)
		{
			//���»�ȡ��ǰ��������
			static DWORD last_interface_num = -1;
			DWORD interface_num;
			GetNumberOfInterfaces(&interface_num);
			if (last_interface_num != -1 && interface_num != last_interface_num)	//��������������仯�������³�ʼ������
			{
				CString info;
				info.LoadString(IDS_CONNECTION_NUM_CHANGED);
				info.Replace(_T("<%before%>"), CCommon::IntToString(last_interface_num));
				info.Replace(_T("<%after%>"), CCommon::IntToString(interface_num));
				info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_restart_cnt + 1));
				IniConnection();
				CCommon::WriteLog(info, theApp.m_log_path.c_str());
			}
			last_interface_num = interface_num;

			string descr;
			descr = (const char*)m_pIfTable->table[m_connections[m_connection_selected].index].bDescr;
			if (descr != theApp.m_cfg_data.m_connection_name)
			{
				//д�����ĵ�����Ϣ
				if (theApp.m_debug_log)
				{
					CString log_str;
					log_str = _T("�������Ʋ�ƥ�䣺\r\n");
					log_str += _T("IfTable description: ");
					log_str += descr.c_str();
					log_str += _T("\r\nm_connection_name: ");
					log_str += theApp.m_cfg_data.m_connection_name.c_str();
					CCommon::WriteLog(log_str, (theApp.m_config_dir + L".\\connections.log").c_str());
				}

				IniConnection();
				CString info;
				info.LoadString(IDS_CONNECTION_NOT_MATCH);
				info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_restart_cnt));
				CCommon::WriteLog(info, theApp.m_log_path.c_str());
			}
		}

		//ֻ�������ں�����������������һ����ʾʱ��ִ������Ĵ���
		if (!theApp.m_cfg_data.m_hide_main_window || theApp.m_cfg_data.m_show_task_bar_wnd)
		{
			//��ȡCPUʹ����
			HQUERY hQuery;
			HCOUNTER hCounter;
			DWORD counterType;
			PDH_RAW_COUNTER rawData;

			PdhOpenQuery(NULL, 0, &hQuery);//��ʼ��ѯ
			PdhAddCounter(hQuery, L"\\Processor Information(_Total)\\% Processor Utility", NULL, &hCounter);
			PdhCollectQueryData(hQuery);
			PdhGetRawCounterValue(hCounter, &counterType, &rawData);

			if (m_first_get_CPU_utility) {//��Ҫ����������ݲ��ܼ���CPUʹ����
				theApp.m_cpu_usage = 0;
				m_first_get_CPU_utility = false;
			} else {
				PDH_FMT_COUNTERVALUE fmtValue;
				PdhCalculateCounterFromRawValue(hCounter, PDH_FMT_DOUBLE, &rawData, &m_last_rawData, &fmtValue);//����ʹ����
				theApp.m_cpu_usage = fmtValue.doubleValue;//��������
				if (theApp.m_cpu_usage > 100)
					theApp.m_cpu_usage = 100;
			}
			m_last_rawData = rawData;//������һ������
			PdhCloseQuery(hQuery);//�رղ�ѯ

			//��ȡ�ڴ�������
			MEMORYSTATUSEX statex;
			statex.dwLength = sizeof(statex);
			GlobalMemoryStatusEx(&statex);
			theApp.m_memory_usage = statex.dwMemoryLoad;
			theApp.m_used_memory = static_cast<int>((statex.ullTotalPhys - statex.ullAvailPhys) / 1024);
			theApp.m_total_memory  = static_cast<int>(statex.ullTotalPhys / 1024);

			ShowInfo();		//ˢ�´�����Ϣ
	
			//���������ʾ
            if (theApp.m_main_wnd_data.show_tool_tip)
            {
                CString tip_info;
                tip_info = GetMouseTipsInfo();
                m_tool_tips.UpdateTipText(tip_info, this);
            }
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
						if (m_tBarDlg->GetCannotInsertToTaskBar() && m_cannot_intsert_to_task_bar_warning)		//ȷ����ʾ��Ϣֻ����һ��
						{
							//д�������־
							CString info;
							info.LoadString(IDS_CONNOT_INSERT_TO_TASKBAR_ERROR_LOG);
							info.Replace(_T("<%cnt%>"), CCommon::IntToString(m_insert_to_taskbar_cnt));
							info.Replace(_T("<%error_code%>"), CCommon::IntToString(m_tBarDlg->GetErrorCode()));
							CCommon::WriteLog(info, theApp.m_log_path.c_str());
							//����������Ϣ
							MessageBox(CCommon::LoadText(IDS_CONNOT_INSERT_TO_TASKBAR, CCommon::IntToString(m_tBarDlg->GetErrorCode())), NULL, MB_ICONWARNING);
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

		//����Ƿ�Ҫ�����ڴ�ʹ���ʳ�����ʾ
		if (theApp.m_general_data.memory_usage_tip_enable)
		{
			static int last_memory_usage;
			static int notify_time{ -theApp.m_notify_interval };		//��¼�ϴε�����ʾʱ��ʱ��
			if (last_memory_usage < theApp.m_general_data.memory_tip_value && theApp.m_memory_usage >= theApp.m_general_data.memory_tip_value && (m_timer_cnt - notify_time > static_cast<unsigned int>(theApp.m_notify_interval)))
			{
				CString info;
				info.Format(CCommon::LoadText(IDS_MEMORY_UDAGE_EXCEED, _T(" %d%%!")), theApp.m_memory_usage);
				ShowNotifyTip(CCommon::LoadText(_T("TrafficMonitor "), IDS_NOTIFY), info.GetString());
				notify_time = m_timer_cnt;
			}
			last_memory_usage = theApp.m_memory_usage;
		}

		//����Ƿ�Ҫ��������ʹ�ó�����ʾ
		if (theApp.m_general_data.traffic_tip_enable)
		{
			static __int64 last_today_traffic;
			__int64 traffic_tip_value;
			if (theApp.m_general_data.traffic_tip_unit == 0)
				traffic_tip_value = static_cast<__int64>(theApp.m_general_data.traffic_tip_value) * 1024 * 1024;
			else
				traffic_tip_value = static_cast<__int64>(theApp.m_general_data.traffic_tip_value) * 1024 * 1024 * 1024;

			__int64 today_traffic = theApp.m_today_up_traffic + theApp.m_today_down_traffic;
			if (last_today_traffic < traffic_tip_value && today_traffic >= traffic_tip_value)
			{
				CString info = CCommon::LoadText(IDS_TODAY_TRAFFIC_EXCEED, CCommon::DataSizeToString(today_traffic));
				ShowNotifyTip(CCommon::LoadText(_T("TrafficMonitor "), IDS_NOTIFY), info.GetString());
			}
			last_today_traffic = today_traffic;
		}

		UpdateNotifyIconTip();

		m_timer_cnt++;
	}

	if (nIDEvent == DELAY_TIMER)
	{
		AutoSelect();
		KillTimer(DELAY_TIMER);
	}

	if (nIDEvent == TASKBAR_TIMER)
	{
		if (m_tBarDlg != nullptr && ::IsWindow(m_tBarDlg->GetSafeHwnd()))
		{
			m_tBarDlg->AdjustWindowPos();
			m_tBarDlg->Invalidate(FALSE);
		}
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
	//����Ĭ�ϲ˵���
	switch (theApp.m_main_wnd_data.double_click_action)
	{
	case DoubleClickAction::CONNECTION_INFO:
		pContextMenu->SetDefaultItem(ID_NETWORK_INFO);
		break;
	//case DoubleClickAction::HISTORY_TRAFFIC:
	//	pContextMenu->SetDefaultItem(ID_TRAFFIC_HISTORY);
	//	break;
	case DoubleClickAction::SHOW_MORE_INFO:
		pContextMenu->SetDefaultItem(ID_SHOW_CPU_MEMORY);
		break;
	case DoubleClickAction::OPTIONS:
		pContextMenu->SetDefaultItem(ID_OPTIONS);
		break;
	//case DoubleClickAction::CHANGE_SKIN:
	//	pContextMenu->SetDefaultItem(ID_CHANGE_SKIN);
	//	break;
	default:
		pContextMenu->SetDefaultItem(-1);
		break;
	}
	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //��ָ��λ����ʾ�����˵�

	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTrafficMonitorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//��δ��������λ��ʱ����ͨ����������ڲ����϶�����
	if (!theApp.m_cfg_data.m_lock_window_pos)
		PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CTrafficMonitorDlg::OnNetworkInfo()
{
	// TODO: �ڴ���������������
	//�������������顱�Ի���
	CNetworkInfoDlg aDlg(m_connections, m_pIfTable->table, m_connection_selected);
	////��CNetworkInfoDlg�ഫ�������������ѷ��ͺͽ��յ��ֽ���
	//aDlg.m_in_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwInOctets - m_connections[m_connection_selected].in_bytes;
	//aDlg.m_out_bytes = m_pIfTable->table[m_connections[m_connection_selected].index].dwOutOctets - m_connections[m_connection_selected].out_bytes;
	aDlg.m_start_time = m_start_time;
	aDlg.DoModal();
	//SetAlwaysOnTop();	//�����ڡ��������顱�Ի�����������ȡ�������ö��������ڶԻ���رպ��������ô����ö�
	if(m_tBarDlg!=nullptr)
		m_tBarDlg->m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);	//�����������������ڵ���ʾ��Ϣ�ö�
}


void CTrafficMonitorDlg::OnAlwaysOnTop()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_always_on_top = !theApp.m_cfg_data.m_always_on_top;
	SetAlwaysOnTop();
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateAlwaysOnTop(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_always_on_top);
}


//�˺�������ʹ�� OnUpdatexxxxx(CCmdUI *pCmdUI) �����ڻ��ڶԻ����������Ч
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
	theApp.m_cfg_data.m_transparency = 100;
	SetTransparency();
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency80()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_transparency = 80;
	SetTransparency();
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency60()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_transparency = 60;
	SetTransparency();
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnTransparency40()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_transparency = 40;
	SetTransparency();
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	theApp.m_cannot_save_config_warning = true;
	theApp.m_cannot_save_global_config_warning = true;
	theApp.SaveConfig();	//�˳�ǰ�������õ�ini�ļ�
	theApp.SaveGlobalConfig();
	SaveHistoryTraffic();

	if (m_tBarDlg != nullptr)
		m_tBarDlg->OnCancel();

	CDialogEx::OnClose();
}


BOOL CTrafficMonitorDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	UINT uMsg = LOWORD(wParam);
	if (uMsg == ID_SELECT_ALL_CONNECTION)
	{
		theApp.m_cfg_data.m_select_all = true;
		theApp.m_cfg_data.m_auto_select = false;
		m_connection_change_flag = true;
	}
	//ѡ���ˡ�ѡ���������ӡ��Ӳ˵�����Ŀʱ�Ĵ���
	if (uMsg == ID_SELETE_CONNECTION)	//ѡ���ˡ��Զ�ѡ�񡱲˵���
	{
		AutoSelect();
		theApp.m_cfg_data.m_auto_select = true;
		theApp.m_cfg_data.m_select_all = false;
		theApp.SaveConfig();
		m_connection_change_flag = true;
	}
	if (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELECT_ALL_CONNECTION + m_connections.size())	//ѡ����һ����������
	{
		m_connection_selected = uMsg - ID_SELECT_ALL_CONNECTION - 1;
		theApp.m_cfg_data.m_connection_name = m_connections[m_connection_selected].description_2;
		theApp.m_cfg_data.m_auto_select = false;
		theApp.m_cfg_data.m_select_all = false;
		theApp.SaveConfig();
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
	CMenu* select_connection_menu = m_menu.GetSubMenu(0)->GetSubMenu(0);
	SetConnectionMenuState(select_connection_menu);

	//���á����ڲ�͸���ȡ��Ӳ˵��¸���ѡ���ѡ��״̬
	switch (theApp.m_cfg_data.m_transparency)
	{
	case 100: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_100, MF_BYCOMMAND | MF_CHECKED); break;
	case 80: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_80, MF_BYCOMMAND | MF_CHECKED); break;
	case 60: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_60, MF_BYCOMMAND | MF_CHECKED); break;
	case 40: pMenu->CheckMenuRadioItem(ID_TRANSPARENCY_100, ID_TRANSPARENCY_40, ID_TRANSPARENCY_40, MF_BYCOMMAND | MF_CHECKED); break;
	default: break;
	}

	if(!theApp.m_cfg_data.m_show_task_bar_wnd && (theApp.m_cfg_data.m_hide_main_window || theApp.m_cfg_data.m_mouse_penetrate))	//���û����ʾ���������ڣ��������������ڻ���������괩͸������á���ʾ֪ͨ��ͼ�ꡱ�˵���
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_GRAYED);
	else
		pMenu->EnableMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | MF_ENABLED);

	pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface? MF_GRAYED : MF_ENABLED));

	//pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


BOOL CTrafficMonitorDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//���ΰ��س�����ESC���˳�
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	if (theApp.m_main_wnd_data.show_tool_tip && m_tool_tips.GetSafeHwnd())
	{
		m_tool_tips.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTrafficMonitorDlg::OnLockWindowPos()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_lock_window_pos = !theApp.m_cfg_data.m_lock_window_pos;
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateLockWindowPos(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_lock_window_pos);
}


void CTrafficMonitorDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	// TODO: �ڴ˴������Ϣ����������

	if (!m_first_start)
	{
		theApp.m_cfg_data.m_position_x = x;
		theApp.m_cfg_data.m_position_y = y;
	}

	//ȷ�����ڲ��ᳬ����Ļ��Χ
	CheckWindowPos();
}



afx_msg LRESULT CTrafficMonitorDlg::OnNotifyIcon(WPARAM wParam, LPARAM lParam)
{
	bool dialog_exist{ false };
	HWND handle{};
	if (lParam == WM_LBUTTONDOWN || lParam == WM_RBUTTONUP || lParam == WM_LBUTTONDBLCLK)
	{
		const int WIND_NUM{ 7 };
		//const CString diloge_title[WIND_NUM]{ _T("���� TrafficMonitor"),_T("����"), _T("��ʷ����ͳ��"), _T("��������"), _T("����Ƥ��"), _T("ѡ������"), _T("����֪ͨ��ͼ��") };
		const CString diloge_title[WIND_NUM]{ CCommon::LoadText(IDS_TITLE_ABOUT), CCommon::LoadText(IDS_TITLE_DONATE), CCommon::LoadText(IDS_TITLE_HISTORY_TRAFFIC),
			CCommon::LoadText(IDS_TITLE_CONNECTION_DETIAL), CCommon::LoadText(IDS_TITLE_CHANGE_SKIN), CCommon::LoadText(IDS_TITLE_OPTION), CCommon::LoadText(IDS_TITLE_CHANGE_ICON) };
		//���β��ҳ����е�ÿһ���Ի�������ҵ�һ��û�йرյĶԻ���ʱ�����������Ҽ��˵�����ֹ�û��ڴ�ʱ�˳�����
		for (int i{}; i < WIND_NUM; i++)
		{
			handle = ::FindWindow(NULL, diloge_title[i]);
			if (handle != NULL)
			{
				HWND hParent = ::GetParent(handle);		//�����ҵ��Ĵ��ڵĸ����ڵľ��
				if (hParent == m_hWnd || (m_tBarDlg!=nullptr && hParent==m_tBarDlg->m_hWnd))			//ֻ�е��ҵ��Ĵ��ڵĸ������ǳ��������ڻ�����������ʱ����˵���ҵ���
				{
					dialog_exist = true;
					break;
				}
			}
		}

	}

	if (lParam == WM_LBUTTONDOWN)
	{
		if (!theApp.m_cfg_data.m_hide_main_window)
		{
			if (dialog_exist)		//�д򿪵ĶԻ���ʱ�����֪ͨ��ͼ��󽫽������õ��Ի���
			{
				::SetForegroundWindow(handle);
			}
			else		//û�д򿪵ĶԻ���ʱ������ʾ������
			{
				ShowWindow(SW_RESTORE);
				theApp.m_cfg_data.m_hide_main_window = false;
				SetForegroundWindow();
				SetAlwaysOnTop();
				CheckWindowPos();
				theApp.SaveConfig();
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
			theApp.m_cfg_data.m_hide_main_window = false;
			SetForegroundWindow();
			SetAlwaysOnTop();
			CheckWindowPos();
			theApp.SaveConfig();
		}
	}
	return 0;
}


void CTrafficMonitorDlg::OnShowNotifyIcon()
{
	// TODO: �ڴ���������������
	if (theApp.m_cfg_data.m_show_notify_icon)
	{
		DeleteNotifyIcon();
		theApp.m_cfg_data.m_show_notify_icon = false;
	}
	else
	{
		AddNotifyIcon();
		theApp.m_cfg_data.m_show_notify_icon = true;
	}
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateShowNotifyIcon(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_show_notify_icon);
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
	if (theApp.m_cfg_data.m_show_more_info)
	{
		rect.right = rect.left + m_layout_data.width_s;
		rect.bottom = rect.top + m_layout_data.height_s;
		MoveWindow(rect);
		CheckWindowPos();
		theApp.m_cfg_data.m_show_more_info = false;
	}
	else
	{
		rect.right = rect.left + m_layout_data.width_l;
		rect.bottom = rect.top + m_layout_data.height_l;
		MoveWindow(rect);
		CheckWindowPos();
		theApp.m_cfg_data.m_show_more_info = true;
	}
	LoadBackGroundImage();
	SetItemPosition();
	ShowInfo();
	theApp.SaveConfig();
}


//�����������л���ʾCPU���ڴ�������ʱ�Ĵ���
void CTrafficMonitorDlg::OnShowCpuMemory2()
{
	// TODO: �ڴ���������������
	if (m_tBarDlg != nullptr)
	{
		theApp.m_cfg_data.m_tbar_show_cpu_memory = !theApp.m_cfg_data.m_tbar_show_cpu_memory;
		//�л���ʾCPU���ڴ�������ʱ��ɾ�����������ڣ���������ʾ
		CloseTaskBarWnd();
		OpenTaskBarWnd();
	}
}


void CTrafficMonitorDlg::OnUpdateShowCpuMemory(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_show_more_info);
}


void CTrafficMonitorDlg::OnMousePenetrate()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_mouse_penetrate = !theApp.m_cfg_data.m_mouse_penetrate;
	SetMousePenetrate();
	if (theApp.m_cfg_data.m_mouse_penetrate && !theApp.m_cfg_data.m_show_notify_icon)	//��괩͸ʱ�����֪ͨͼ��û����ʾ��������ʾ�����������޷������Ҽ��˵�
	{
		//���֪ͨ��ͼ��
		AddNotifyIcon();
		theApp.m_cfg_data.m_show_notify_icon = true;
	}
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateMousePenetrate(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_mouse_penetrate);
}


void CTrafficMonitorDlg::OnShowTaskBarWnd()
{
	// TODO: �ڴ���������������
	if (m_tBarDlg != nullptr)
	{
		CloseTaskBarWnd();
	}
	if (!theApp.m_cfg_data.m_show_task_bar_wnd)
	{
		theApp.m_cfg_data.m_show_task_bar_wnd = true;
		OpenTaskBarWnd();
	}
	else
	{
		theApp.m_cfg_data.m_show_task_bar_wnd = false;
		//�ر����������ں����û����ʾ֪ͨ��ͼ�꣬��û����ʾ�����ڻ���������괩͸����֪ͨ��ͼ����ʾ����
		if (!theApp.m_cfg_data.m_show_notify_icon && (theApp.m_cfg_data.m_hide_main_window || theApp.m_cfg_data.m_mouse_penetrate))
		{
			AddNotifyIcon();
			theApp.m_cfg_data.m_show_notify_icon = true;
		}
	}
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateShowTaskBarWnd(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_show_task_bar_wnd);
}


void CTrafficMonitorDlg::OnAppAbout()
{
	// TODO: �ڴ���������������
	//���������ڡ��Ի���
	CAboutDlg aDlg;
	aDlg.DoModal();
}


//����Դ����������ʱ�ᴥ������Ϣ
LRESULT CTrafficMonitorDlg::OnTaskBarCreated(WPARAM wParam, LPARAM lParam)
{
	if (m_tBarDlg != nullptr)
	{
		CloseTaskBarWnd();
		if (theApp.m_cfg_data.m_show_notify_icon)
		{
			//�������֪ͨ��ͼ��
			::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
		}
		OpenTaskBarWnd();
	}
	else
	{
		if (theApp.m_cfg_data.m_show_notify_icon)
			::Shell_NotifyIcon(NIM_ADD, &m_ntIcon);
	}
	return LRESULT();
}


void CTrafficMonitorDlg::OnShowMainWnd()
{
	// TODO: �ڴ���������������
	if (!theApp.m_cfg_data.m_hide_main_window)
	{
		ShowWindow(SW_HIDE);
		theApp.m_cfg_data.m_hide_main_window = true;
		//���������ں����û����ʾ֪ͨ��ͼ�꣬������ʾ����
		if (!theApp.m_cfg_data.m_show_notify_icon && !theApp.m_cfg_data.m_show_task_bar_wnd)
		{
			AddNotifyIcon();
			theApp.m_cfg_data.m_show_notify_icon = true;
		}
	}
	else
	{
		ShowWindow(SW_RESTORE);
		theApp.m_cfg_data.m_hide_main_window = false;
	}
	theApp.SaveConfig();
}


void CTrafficMonitorDlg::OnUpdateShowMainWnd(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(!theApp.m_cfg_data.m_hide_main_window);
}


void CTrafficMonitorDlg::OnChangeSkin()
{
	// TODO: �ڴ���������������
	CSkinDlg skinDlg;
	//��ʼ��CSkinDlg���������
	skinDlg.m_skins = m_skins;
	skinDlg.m_skin_selected = m_skin_selected;
	skinDlg.m_pFont = &m_font;
	if (skinDlg.DoModal() == IDOK)
	{
		m_skin_selected = skinDlg.m_skin_selected;
		theApp.m_cfg_data.m_skin_name = m_skins[m_skin_selected];
		//��ȡƤ������
		GetSkinLayout();
		//���뱳��ͼƬ
		LoadBackGroundImage();
		//��ȡƤ����������ɫ
		theApp.m_main_wnd_data.specify_each_item_color = skinDlg.GetSkinData().specify_each_item_color;
		for(int i{}; i<MAIN_WND_COLOR_NUM; i++)
			theApp.m_main_wnd_data.text_colors[i] = skinDlg.GetSkinData().text_colors[i];
		SetTextColor();
		//��ȡƤ��������
		if (theApp.m_general_data.allow_skin_cover_font)
		{
			if (!skinDlg.GetSkinData().font.name.IsEmpty())
			{
				theApp.m_main_wnd_data.font.name = skinDlg.GetSkinData().font.name;
				theApp.m_main_wnd_data.font.bold = skinDlg.GetSkinData().font.bold;
				theApp.m_main_wnd_data.font.italic = skinDlg.GetSkinData().font.italic;
				theApp.m_main_wnd_data.font.underline = skinDlg.GetSkinData().font.underline;
				theApp.m_main_wnd_data.font.strike_out = skinDlg.GetSkinData().font.strike_out;
			}
			if (skinDlg.GetSkinData().font.size >= MIN_FONT_SIZE && skinDlg.GetSkinData().font.size <= MAX_FONT_SIZE)
				theApp.m_main_wnd_data.font.size = skinDlg.GetSkinData().font.size;
			SetTextFont();
		}
		//��ȡ��Ŀ����ʾ�ı�
		if (theApp.m_general_data.allow_skin_cover_text)
		{
			theApp.m_main_wnd_data.disp_str = skinDlg.GetSkinData().disp_str;
		}
		SetItemPosition();
		ShowInfo();		//����Ƥ��������ˢ�´�����Ϣ
		theApp.SaveConfig();
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
	switch (theApp.m_main_wnd_data.double_click_action)
	{
	case DoubleClickAction::CONNECTION_INFO:
		OnNetworkInfo();			//˫���󵯳����������顱�Ի���
		break;
	case DoubleClickAction::HISTORY_TRAFFIC:
		OnTrafficHistory();			//˫���󵯳�����ʷ����ͳ�ơ��Ի���
		break;
	case DoubleClickAction::SHOW_MORE_INFO:
		OnShowCpuMemory();			//�л���ʾCPU���ڴ�������
		break;
	case DoubleClickAction::OPTIONS:
		OnOptions();				//˫���󵯳���ѡ�����á��Ի���
		break;
	case DoubleClickAction::TASK_MANAGER:
		ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir + L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);		//�����������
		break;
	case DoubleClickAction::CHANGE_SKIN:
		OnChangeSkin();				//˫���󵯳�������Ƥ�����Ի���
		break;
	default:
		break;
	}
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
	CIconSelectDlg dlg(theApp.m_cfg_data.m_notify_icon_selected);
	if (dlg.DoModal() == IDOK)
	{
		theApp.m_cfg_data.m_notify_icon_selected = dlg.GetIconSelected();
		m_ntIcon.hIcon = theApp.m_notify_icons[theApp.m_cfg_data.m_notify_icon_selected];
		if (theApp.m_cfg_data.m_show_notify_icon)
		{
			DeleteNotifyIcon();
			AddNotifyIcon();
		}
		theApp.SaveConfig();
	}
}


void CTrafficMonitorDlg::OnAlowOutOfBorder()
{
	// TODO: �ڴ���������������
	theApp.m_cfg_data.m_alow_out_of_border = !theApp.m_cfg_data.m_alow_out_of_border;
	CheckWindowPos();
}


void CTrafficMonitorDlg::OnUpdateAlowOutOfBorder(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(theApp.m_cfg_data.m_alow_out_of_border);
}


void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
}


void CTrafficMonitorDlg::OnCheckUpdate()
{
	// TODO: �ڴ���������������
	theApp.CheckUpdate(true);
}


afx_msg LRESULT CTrafficMonitorDlg::OnTaskbarMenuPopedUp(WPARAM wParam, LPARAM lParam)
{
	//���á�ѡ�����ӡ��Ӳ˵����и���ѡ���ѡ��״̬
	CMenu* select_connection_menu = m_tBarDlg->m_menu.GetSubMenu(0)->GetSubMenu(0);
	SetConnectionMenuState(select_connection_menu);
	return 0;
}
