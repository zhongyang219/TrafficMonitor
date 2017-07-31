// TaskBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"


// CTaskBarDlg 对话框

IMPLEMENT_DYNAMIC(CTaskBarDlg, CDialogEx)

CTaskBarDlg::CTaskBarDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TASK_BAR_DIALOG, pParent)
{

}

CTaskBarDlg::~CTaskBarDlg()
{
}

void CTaskBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_STATIC_UP1, m_disp_up);
	//DDX_Control(pDX, IDC_STATIC_DOWN1, m_disp_down);
	//DDX_Control(pDX, IDC_STATIC3, m_disp_cpu);
	//DDX_Control(pDX, IDC_STATIC4, m_disp_memory);
}


BEGIN_MESSAGE_MAP(CTaskBarDlg, CDialogEx)
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_SET_BACK_COLOR, &CTaskBarDlg::OnSetBackgroundColor)
	ON_COMMAND(ID_SET_TEXT_COLOR, &CTaskBarDlg::OnSetTextColor)
	ON_WM_INITMENU()
	ON_COMMAND(ID_SET_FONT2, &CTaskBarDlg::OnSetFont)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTaskBarDlg 消息处理程序


void CTaskBarDlg::ShowInfo()
{
	if (this->m_hWnd == NULL || m_pDC == nullptr) return;
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed);
	
	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	int draw_width = (m_show_cpu_memory ? m_window_width : m_window_width_s);
	MemBitmap.CreateCompatibleBitmap(m_pDC, draw_width, m_rect.Height());
	MemDC.SelectObject(&MemBitmap);
	//绘图
	CRect tmp{ m_rect };
	tmp.MoveToXY(0, 0);
	MemDC.FillSolidRect(tmp, m_back_color);		//填充背景色
	MemDC.SelectObject(&m_font);
	//绘制上传速度
	tmp.bottom = m_window_height / 2;
	tmp.right = tmp.left + m_window_width_s;
	if (!m_swap_up_down)
		str.Format(_T("↑:%s/s"), out_speed.GetString());
	else
		str.Format(_T("↓:%s/s"), in_speed.GetString());
	CCommon::DrawWindowText(&MemDC, tmp, str, m_text_color, m_back_color);
	//绘制下载速度
	tmp.MoveToY(tmp.bottom);
	if (!m_swap_up_down)
		str.Format(_T("↓:%s/s"), in_speed.GetString());
	else
		str.Format(_T("↑:%s/s"), out_speed.GetString());
	CCommon::DrawWindowText(&MemDC, tmp, str, m_text_color, m_back_color);
	if (m_show_cpu_memory)
	{
		//绘制CPU利用率
		if (!m_taskbar_on_side)
		{
			tmp.MoveToXY(m_window_width_s, 0);
			tmp.right = tmp.left + (m_window_width - m_window_width_s);
		}
		else
		{
			tmp.MoveToY(m_window_height);
			tmp.right = tmp.left + (m_window_width - m_window_width_s);
		}
		str.Format(_T("CPU:%d%%"), theApp.m_cpu_usage);
		CCommon::DrawWindowText(&MemDC, tmp, str, m_text_color, m_back_color);
		//绘制内存利用率
		tmp.MoveToY(tmp.bottom);
		str.Format(_T("内存:%d%%"), theApp.m_memory_usage);
		CCommon::DrawWindowText(&MemDC, tmp, str, m_text_color, m_back_color);
	}
	//将缓冲区DC中的图像拷贝到屏幕中显示
	m_pDC->BitBlt(0,0, draw_width, m_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();



	//if (!m_swap_up_down)
	//{
	//	str.Format(_T("↑:%s/s"), out_speed.GetString());
	//	m_disp_up.SetWindowTextEx(str);
	//	str.Format(_T("↓:%s/s"), in_speed.GetString());
	//	m_disp_down.SetWindowTextEx(str);
	//}
	//else
	//{
	//	str.Format(_T("↓:%s/s"), in_speed.GetString());
	//	m_disp_up.SetWindowTextEx(str);
	//	str.Format(_T("↑:%s/s"), out_speed.GetString());
	//	m_disp_down.SetWindowTextEx(str);
	//}
	//if (m_show_cpu_memory)
	//{
	//	str.Format(_T("CPU:%d%%"), theApp.m_cpu_usage);
	//	m_disp_cpu.SetWindowTextEx(str);
	//	str.Format(_T("内存:%d%%"), theApp.m_memory_usage);
	//	m_disp_memory.SetWindowTextEx(str);
	//}
	//else
	//{
	//	m_disp_cpu.SetWindowText(_T(""));
	//	m_disp_memory.SetWindowText(_T(""));
	//}
}


bool CTaskBarDlg::AdjustWindowPos()
{
	//CRect rect = m_rcMin;
	//rect.right = rect.left + m_min_bar_width;
	//::MoveWindow(m_hMin, 0, 0, rect.Width(), rect.Height(), TRUE);
	if (this->m_hWnd == NULL)
		return false;
	CRect rcMin;
	::GetWindowRect(m_hMin, rcMin);	//获得最小化窗口的区域
	if (rcMin.Width() >= rcMin.Height())		//如果任务栏的宽度大于高度，即任务栏在桌面顶部或底部时
	{
		//设置窗口大小
		m_rect.right = m_rect.left + (m_show_cpu_memory ? m_window_width : m_window_width_s);
		m_rect.bottom = m_rect.top + m_window_height;
		if (rcMin.Width() != m_min_bar_width)	//如果最小化窗口的宽度改变了，重新设置任务栏窗口的位置
		{
			m_rcMin = rcMin;
			m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//保存最小化窗口宽度
			::MoveWindow(m_hMin, 0, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);	//设置最小化窗口的位置
			::MoveWindow(this->m_hWnd, m_rcMin.Width() - m_rect.Width() + 2, (m_rcMin.Height() - m_rect.Height()) / 2, \
				m_rect.Width(), m_rect.Height(), TRUE);		//设置任务栏窗口的位置

			CRect rect;
			GetWindowRect(rect);	//获取当前窗口的绝对位置
			//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
			if (rect.left == m_rcMin.Width() - m_rect.Width() + 2 && rect.top == (m_rcMin.Height() - m_rect.Height()) / 2)
			{
				rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
				this->MoveWindow(rect);
			}
		}
		m_taskbar_on_side = false;
	}
	else		//当任务栏在屏幕在左侧或右侧时
	{
		//设置窗口大小
		m_rect.right = m_rect.left + m_window_width_s;
		m_rect.bottom = m_rect.top + (m_show_cpu_memory ? (2 * m_window_height) : m_window_height);
		if (rcMin.Height() != m_min_bar_height)	//如果最小化窗口的高度改变了，重新设置任务栏窗口的位置
		{
			m_rcMin = rcMin;
			m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//保存最小化窗口高度
			::MoveWindow(m_hMin, 0, 0, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//设置最小化窗口的位置
			int left_pos = (m_rcMin.Width() - m_window_width_s) / 2;
			if (left_pos < 0) left_pos = 0;
			::MoveWindow(this->m_hWnd, left_pos, m_rcMin.Height() - m_rect.Height() + 2, \
				m_rect.Width(), m_rect.Height(), TRUE);		//设置任务栏窗口的位置

			CRect rect;
			GetWindowRect(rect);	//获取当前窗口的绝对位置
			//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
			if (rect.left == left_pos && rect.top == m_rcMin.Height() - m_rect.Height() + 2)
			{
				rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
				this->MoveWindow(rect);
			}
		}
		m_taskbar_on_side = true;
	}
	if (m_connot_insert_to_task_bar && ::GetForegroundWindow() == m_hTaskbar)	//在窗口无法嵌入任务栏时，如果焦点设置在了任务栏上，则让窗口置顶
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//设置置顶
	}
	return true;
}


void CTaskBarDlg::SaveConfig()
{
	CCommon::WritePrivateProfileIntW(L"task_bar", L"task_bar_back_color", m_back_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"task_bar_text_color", m_text_color, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"tack_bar_show_cpu_memory", m_show_cpu_memory, theApp.m_config_path.c_str());
	WritePrivateProfileString(_T("task_bar"), _T("tack_bar_font_name"), m_font_name, theApp.m_config_path.c_str());
	CCommon::WritePrivateProfileIntW(L"task_bar", L"tack_bar_font_size", m_font_size, theApp.m_config_path.c_str());
}

void CTaskBarDlg::LoadConfig()
{
	m_back_color = GetPrivateProfileInt(_T("task_bar"), _T("task_bar_back_color"), 0, theApp.m_config_path.c_str());
	m_text_color = GetPrivateProfileInt(_T("task_bar"), _T("task_bar_text_color"), 0x00ffffffU, theApp.m_config_path.c_str());
	m_show_cpu_memory = (GetPrivateProfileInt(_T("task_bar"), _T("tack_bar_show_cpu_memory"), 1, theApp.m_config_path.c_str()) != 0);
	m_swap_up_down = (GetPrivateProfileInt(_T("config"), _T("swap_up_down"), 0, theApp.m_config_path.c_str()) != 0);
	GetPrivateProfileString(_T("task_bar"), _T("tack_bar_font_name"), _T("微软雅黑"), m_font_name.GetBuffer(32), 32, theApp.m_config_path.c_str());
	m_font_size = GetPrivateProfileInt(_T("task_bar"), _T("tack_bar_font_size"), 9, theApp.m_config_path.c_str());
}

BOOL CTaskBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//设置隐藏任务栏图标
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	LoadConfig();

	this->GetWindowRect(m_rect);
	m_window_width = m_rect.Width();
	m_window_width_s = static_cast<int>(m_rect.Width()*0.566f + 0.5f);
	m_window_height = m_rect.Height();
	if (!m_show_cpu_memory)
	{
		m_rect.right = m_rect.left + m_window_width_s;
	}

	m_hTaskbar = ::FindWindow(L"Shell_TrayWnd", NULL);		//寻找类名是Shell_TrayWnd的窗口句柄
	m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL);	//寻找二级容器的句柄
	m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);	//寻找最小化窗口的句柄

	::GetWindowRect(m_hMin, m_rcMin);	//获得最小化窗口的区域
	::GetWindowRect(m_hBar, m_rcBar);	//获得二级容器的区域

	if (m_rcMin.Width() >= m_rcMin.Height())		//如果任务栏的宽度大于高度，即任务栏在桌面顶部或底部时
	{
		m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//保存最小化窗口宽度

		//通过用MoveWindow函数来改变小化窗口的宽度
		::MoveWindow(m_hMin, 0, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//把程序窗口设置成任务栏的子窗口

		//调整程序窗口的大小和位置
		//注：当把当前窗口设置为任务栏的子窗口后，MoveWindow函数移动的位置是基于任务栏的相对位置，
		//在某些情况下，如被安全软件阻止时，窗口无法嵌入任务栏，窗口会移动到基于屏幕左上角的绝对位置。
		::MoveWindow(this->m_hWnd, m_rcMin.Width() - m_rect.Width() + 2, (m_rcBar.Height() - m_rect.Height()) / 2, \
			m_rect.Width(), m_rect.Height(), TRUE);

		CRect rect;
		GetWindowRect(rect);	//获取当前窗口的绝对位置
		//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
		if (rect.left == m_rcMin.Width() - m_rect.Width() + 2 && rect.top == (m_rcBar.Height() - m_rect.Height()) / 2)
		{
			rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
			this->MoveWindow(rect);
			m_connot_insert_to_task_bar = true;
			MessageBox(_T("警告：窗口没有成功嵌入任务栏，可能已被安全软件阻止！"), NULL, MB_ICONWARNING);
		}
		m_taskbar_on_side = false;
	}
	else	//当任务栏在桌面左侧或右侧时
	{
		int left_pos = (m_rcMin.Width() - m_window_width_s) / 2;
		if (left_pos < 0) left_pos = 0;
		if (m_show_cpu_memory)	//将CPU和内存利用率放到网速的下面
		{
			//m_disp_cpu.SetWindowPos(&CWnd::wndTop, 0, m_rect.Height(), 0, 0, SWP_NOSIZE);
			//m_disp_memory.SetWindowPos(&CWnd::wndTop, 0, m_rect.Height() * 3 / 2, 0, 0, SWP_NOSIZE);
			m_rect.right = m_rect.left + m_window_width_s;
			m_rect.bottom = m_rect.top + m_rect.Height() * 2;
		}
		m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//保存最小化窗口高度

		//通过用MoveWindow函数来改变小化窗口的高度
		::MoveWindow(m_hMin, 0, 0, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//把程序窗口设置成任务栏的子窗口

		//调整程序窗口的大小和位置
		//注：当把当前窗口设置为任务栏的子窗口后，MoveWindow函数移动的位置是基于任务栏的相对位置，
		//在某些情况下，如被安全软件阻止时，窗口无法嵌入任务栏，窗口会移动到基于屏幕左上角的绝对位置。
		::MoveWindow(this->m_hWnd, left_pos, m_rcMin.Height() - m_rect.Height() + 2, \
			m_rect.Width(), m_rect.Height(), TRUE);

		CRect rect;
		GetWindowRect(rect);	//获取当前窗口的绝对位置
		//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
		if (rect.left == left_pos && rect.top == m_rcMin.Height() - m_rect.Height() + 2)
		{
			rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
			this->MoveWindow(rect);
			m_connot_insert_to_task_bar = true;
			MessageBox(_T("警告：窗口没有成功嵌入任务栏，可能已被安全软件阻止！"), NULL, MB_ICONWARNING);
		}
		m_taskbar_on_side = true;
	}

	m_menu.LoadMenu(IDR_TASK_BAR_MENU);

	SetBackgroundColor(m_back_color);


	//设置字体
	m_font.CreatePointFont(m_font_size * 10, m_font_name);

	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//设置提示信息总是置顶

	m_pDC = GetDC();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	SaveConfig();
	DestroyWindow();
	//程序关闭的时候，把最小化窗口的width恢复回去
	::MoveWindow(m_hMin, 0, 0, m_rcMin.Width(), m_rcMin.Height(), TRUE);

	//CDialogEx::OnCancel();
}


void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CPoint point1;	//定义一个用于确定光标位置的位置  
	GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标  
	m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTaskBarDlg::OnSetBackgroundColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog colorDlg(m_back_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_back_color = colorDlg.GetColor();
		if (m_back_color == m_text_color)
			MessageBox(_T("警告：背景色和文字颜色相同！"),NULL, MB_ICONWARNING);
		SetBackgroundColor(m_back_color);
		ShowInfo();
		SaveConfig();
	}
}

void CTaskBarDlg::OnSetTextColor()
{
	// TODO: 在此添加命令处理程序代码
	CColorDialog colorDlg(m_text_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_text_color = colorDlg.GetColor();
		if (m_back_color == m_text_color)
			MessageBox(_T("警告：文字颜色和背景色相同！"), NULL, MB_ICONWARNING);
		//m_disp_cpu.SetTextColor(m_text_color);
		//m_disp_memory.SetTextColor(m_text_color);
		//m_disp_up.SetTextColor(m_text_color);
		//m_disp_down.SetTextColor(m_text_color);
		ShowInfo();
		SaveConfig();
	}
}


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY2, MF_BYCOMMAND | (m_show_cpu_memory ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_HIDE_MAIN_WND, MF_BYCOMMAND | (theApp.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | (theApp.m_show_notify_icon ? MF_CHECKED : MF_UNCHECKED));

	pMenu->SetDefaultItem(ID_NETWORK_INFO);
}


BOOL CTaskBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//屏蔽按回车键和ESC键退出
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	if (m_tool_tips.GetSafeHwnd() && (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_LBUTTONUP ||
		pMsg->message == WM_MOUSEMOVE))
	{
		m_tool_tips.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTaskBarDlg::OnSetFont()
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
		//获取字体信息
		m_font_name = fontDlg.m_cf.lpLogFont->lfFaceName;
		m_font_size = fontDlg.m_cf.iPointSize / 10;
		//刷新界面信息
		ShowInfo();
		//将字体设置写入到ini文件
		SaveConfig();
	}
}


void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
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
	//m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);		//设置提示信息总是置顶

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SendMessage(WM_COMMAND,ID_NETWORK_INFO);		//双击后弹出“连接详情”对话框
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
