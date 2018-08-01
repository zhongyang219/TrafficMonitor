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
}


BEGIN_MESSAGE_MAP(CTaskBarDlg, CDialogEx)
	ON_WM_RBUTTONUP()
	ON_WM_INITMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CTaskBarDlg 消息处理程序


void CTaskBarDlg::ShowInfo(CDC* pDC)
{
	if (this->m_hWnd == NULL || pDC == nullptr) return;
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_taskbar_data);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_taskbar_data);

	if (m_rect.IsRectEmpty() || m_rect.IsRectNull()) return;

	//设置要绘制的文本颜色
	COLORREF text_colors[TASKBAR_COLOR_NUM];
	if (theApp.m_taskbar_data.specify_each_item_color)
	{
		for (int i{}; i < TASKBAR_COLOR_NUM; i++)
			text_colors[i] = theApp.m_taskbar_data.text_colors[i];
	}
	else
	{
		for (int i{}; i < TASKBAR_COLOR_NUM; i++)
			text_colors[i] = theApp.m_taskbar_data.text_colors[0];
	}

	//设置缓冲的DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	int draw_width = (theApp.m_cfg_data.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
	MemBitmap.CreateCompatibleBitmap(pDC, draw_width, m_rect.Height());
	MemDC.SelectObject(&MemBitmap);
	//绘图
	CRect value_rect{ m_rect };		//显示数值的矩形区域
	CRect lable_rect;				//显示文本标签的矩形区域
	value_rect.MoveToXY(0, 0);
	MemDC.FillSolidRect(value_rect, theApp.m_taskbar_data.back_color);		//填充背景色
	CDrawCommon draw;
	draw.Create(&MemDC, nullptr);
	draw.SetFont(&m_font);
	draw.SetBackColor(theApp.m_taskbar_data.back_color);
	Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? Alignment::RIGHT : Alignment::LEFT };		//数值的对齐方式
	//绘制上传速度
	if (theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom)
	{
		value_rect.right = m_up_lable_width + m_ud_value_width;
		lable_rect = value_rect;
		lable_rect.right = lable_rect.left + m_up_lable_width;
		value_rect.left += m_up_lable_width;
	}
	else
	{
		value_rect.bottom = m_window_height / 2;
		if (m_taskbar_on_top_or_bottom)
		{
			value_rect.right = value_rect.left + m_window_width_s - theApp.DPI(5);
		}
		else
		{
			int window_width;
			window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
			value_rect.right = value_rect.left + window_width - theApp.DPI(5);
		}
		lable_rect = value_rect;
		lable_rect.right = lable_rect.left + max(m_up_lable_width, m_down_lable_width);
		value_rect.left += max(m_up_lable_width, m_down_lable_width);
	}
	CString format_str;
	if (theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO)
		format_str = _T("%s");
	else
		format_str = _T("%s/s");
	if (!theApp.m_taskbar_data.swap_up_down)
	{
		str.Format(format_str, out_speed.GetString());
		draw.DrawWindowText(value_rect, str, text_colors[1], value_alignment, true);
	}
	else
	{
		str.Format(format_str, in_speed.GetString());
		draw.DrawWindowText(value_rect, str, text_colors[3], value_alignment, true);
	}
	//绘制上传标签
	if (!theApp.m_taskbar_data.swap_up_down)
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.up.c_str(), text_colors[0], Alignment::LEFT, true);
	else
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.down.c_str(), text_colors[2], Alignment::LEFT, true);
	//绘制下载速度
	if (theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom)
	{
		lable_rect.MoveToX(value_rect.right + theApp.DPI(4));
		lable_rect.right = lable_rect.left + m_down_lable_width;
		value_rect.MoveToX(lable_rect.right);
	}
	else
	{
		value_rect.MoveToY(value_rect.bottom);
		lable_rect.MoveToY(lable_rect.bottom);
	}
	if (!theApp.m_taskbar_data.swap_up_down)
	{
		str.Format(format_str, in_speed.GetString());
		draw.DrawWindowText(value_rect, str, text_colors[3], value_alignment, true);
	}
	else
	{
		str.Format(format_str, out_speed.GetString());
		draw.DrawWindowText(value_rect, str, text_colors[1], value_alignment, true);
	}
	//绘制下载标签
	if (!theApp.m_taskbar_data.swap_up_down)
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.down.c_str(), text_colors[2], Alignment::LEFT, true);
	else
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.up.c_str(), text_colors[0], Alignment::LEFT, true);

	if (theApp.m_cfg_data.m_tbar_show_cpu_memory)
	{
		//绘制CPU利用率
		if (theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom)
		{
			value_rect.MoveToXY(m_window_width_s + theApp.DPI(4), 0);
			value_rect.right = value_rect.left + m_cpu_lable_width + m_cm_value_width - theApp.DPI(5);
			lable_rect = value_rect;
			lable_rect.right = lable_rect.left + m_cpu_lable_width;
			value_rect.left += m_cpu_lable_width;
		}
		else if (m_taskbar_on_top_or_bottom)
		{
			value_rect.MoveToXY(m_window_width_s, 0);
			value_rect.right = value_rect.left + (m_window_width - m_window_width_s) - theApp.DPI(5);
			lable_rect = value_rect;
			lable_rect.right = lable_rect.left + max(m_cpu_lable_width, m_memory_lable_width);
			value_rect.left += max(m_cpu_lable_width, m_memory_lable_width);
		}
		else
		{
			value_rect.MoveToXY(0, m_window_height);
			int window_width;
			window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
			value_rect.right = value_rect.left + window_width - theApp.DPI(5);
			lable_rect = value_rect;
			lable_rect.right = lable_rect.left + max(m_cpu_lable_width, m_memory_lable_width);
			value_rect.left += max(m_cpu_lable_width, m_memory_lable_width);
		}
		CString format_str;
		if (theApp.m_taskbar_data.hide_percent)
			format_str = _T("%d");
		else
			format_str = _T("%d%%");
		str.Format(format_str, theApp.m_cpu_usage);
		CRect rect_tmp{ value_rect };
		if (theApp.m_cpu_usage == 100)		//如果CPU利用为100%，则将矩形框右侧扩大一些，防止显示不全
			rect_tmp.right += theApp.DPI(5);
		draw.DrawWindowText(rect_tmp, str, text_colors[5], value_alignment, true);		//绘制数值
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.cpu.c_str(), text_colors[4], Alignment::LEFT, true);				//绘制标签
		//绘制内存利用率
		if (theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom)
		{
			lable_rect.MoveToX(value_rect.right + theApp.DPI(4));
			lable_rect.right = lable_rect.left + m_memory_lable_width;
			value_rect.MoveToX(lable_rect.right);
		}
		else
		{
			value_rect.MoveToY(value_rect.bottom);
			lable_rect.MoveToY(lable_rect.bottom);
		}
		str.Format(format_str, theApp.m_memory_usage);
		rect_tmp = value_rect;
		if (theApp.m_memory_usage == 100)		//如果内存利用为100%，则将矩形框右侧扩大一些，防止显示不全
			rect_tmp.right += theApp.DPI(5);
		draw.DrawWindowText(rect_tmp, str, text_colors[7], value_alignment, true);
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.memory.c_str(), text_colors[6], Alignment::LEFT, true);
	}
	//将缓冲区DC中的图像拷贝到屏幕中显示
	pDC->BitBlt(0,0, draw_width, m_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}


bool CTaskBarDlg::AdjustWindowPos()
{
	if (this->m_hWnd == NULL)
		return false;
	CRect rcMin, rcBar;
	::GetWindowRect(m_hMin, rcMin);	//获得最小化窗口的区域
	::GetWindowRect(m_hBar, rcBar);	//获得二级容器的区域
	static bool last_taskbar_on_top_or_bottom;
	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	if (m_taskbar_on_top_or_bottom != last_taskbar_on_top_or_bottom)
	{
		CalculateWindowWidth();
		last_taskbar_on_top_or_bottom = m_taskbar_on_top_or_bottom;
	}
	int wnd_x_pos{}, wnd_y_pos{};
	if (m_taskbar_on_top_or_bottom)		//当任务栏在桌面顶部或底部时
	{
		//设置窗口大小
		m_rect.right = m_rect.left + (theApp.m_cfg_data.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
		m_rect.bottom = m_rect.top + m_window_height;
		if (rcMin.Width() != m_min_bar_width)	//如果最小化窗口的宽度改变了，重新设置任务栏窗口的位置
		{
			m_left_space = rcMin.left - rcBar.left;
			m_rcMin = rcMin;
			m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//保存最小化窗口宽度
			if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			{
				::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);	//设置最小化窗口的位置
				wnd_x_pos = m_left_space + m_rcMin.Width() - m_rect.Width() + 2;
			}
			else
			{
				::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
				wnd_x_pos = m_left_space;
			}
			wnd_y_pos = (m_rcBar.Height() - m_rect.Height()) / 2;
			if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
				wnd_y_pos += theApp.DPI(1);
			::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);		//设置任务栏窗口的位置
		}
	}
	else		//当任务栏在屏幕在左侧或右侧时
	{
		//设置窗口大小
		int window_width;
		window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
		m_rect.right = m_rect.left + window_width;
		m_rect.bottom = m_rect.top + (theApp.m_cfg_data.m_tbar_show_cpu_memory ? (2 * m_window_height + theApp.DPI(2)) : m_window_height);
		if (rcMin.Height() != m_min_bar_height)	//如果最小化窗口的高度改变了，重新设置任务栏窗口的位置
		{
			m_top_space = rcMin.top - rcBar.top;
			m_rcMin = rcMin;
			m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//保存最小化窗口高度
			if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			{
				::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//设置最小化窗口的位置
				wnd_y_pos = m_top_space + m_rcMin.Height() - m_rect.Height() + 2;
			}
			else
			{
				::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//设置最小化窗口的位置
				wnd_y_pos = m_top_space;
			}
			wnd_x_pos = (m_rcMin.Width() - window_width) / 2;
			if (wnd_x_pos < theApp.DPI(2)) wnd_x_pos = theApp.DPI(2);
			::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);		//设置任务栏窗口的位置
		}
	}

	CRect rect{};
	GetWindowRect(rect);	//获取当前窗口的绝对位置
	//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
	if (rect.left == wnd_x_pos && rect.top == wnd_y_pos)
	{
		rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
		this->MoveWindow(rect);
	}

	if (m_connot_insert_to_task_bar && ::GetForegroundWindow() == m_hTaskbar)	//在窗口无法嵌入任务栏时，如果焦点设置在了任务栏上，则让窗口置顶
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//设置置顶
	}
	return true;
}


bool CTaskBarDlg::IsTaskbarOnTopOrBottom()
{
	CRect rect;
	CRect rcMin;
	CRect rcBar;
	if (m_hTaskbar != 0)
	{
		::GetWindowRect(m_hMin, rcMin);	//获得最小化窗口的区域
		::GetWindowRect(m_hBar, rcBar);	//获得二级容器的区域
		if(m_left_space==0)
			m_left_space = rcMin.left - rcBar.left;
		if(m_top_space==0)
			m_top_space = rcMin.top - rcBar.top;

		::GetWindowRect(m_hTaskbar, rect);			//获取任务栏的矩形区域
		return (rect.Width()>=rect.Height());		//如果任务栏的宽度大于高度，则任务在屏幕的顶部或底部
	}
	else
	{
		return true;
	}
}

CString CTaskBarDlg::GetMouseTipsInfo()
{
	CString tip_info;
	if (theApp.m_cfg_data.m_tbar_show_cpu_memory)
	{
		tip_info.Format(_T("%s: %s\r\n%s: %s/%s"),
			CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
			CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_traffic / 1024)),
			CCommon::LoadText(IDS_MEMORY_USAGE),
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
	}
	else
	{
		tip_info.Format(_T("%s: %s\r\n%s: %d%%\r\n%s: %s/%s (%d%%)"),
			CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
			CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_traffic / 1024)),
			CCommon::LoadText(IDS_CPU_USAGE),
			theApp.m_cpu_usage,
			CCommon::LoadText(IDS_MEMORY_USAGE),
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory),
			theApp.m_memory_usage);
	}
	return tip_info;
}

void CTaskBarDlg::SetTextFont()
{
	//如果m_font已经关联了一个字体资源对象，则释放它
	if (m_font.m_hObject)
	{
		m_font.DeleteObject();
	}
	//创建新的字体
	m_font.CreateFont(
		FONTSIZE_TO_LFHEIGHT(theApp.m_taskbar_data.font.size), // nHeight
		0, // nWidth
		0, // nEscapement
		0, // nOrientation
		(theApp.m_taskbar_data.font.bold ? FW_BOLD : FW_NORMAL), // nWeight
		theApp.m_taskbar_data.font.italic, // bItalic
		theApp.m_taskbar_data.font.underline, // bUnderline
		theApp.m_taskbar_data.font.strike_out, // cStrikeOut
		DEFAULT_CHARSET, // nCharSet
		OUT_DEFAULT_PRECIS, // nOutPrecision
		CLIP_DEFAULT_PRECIS, // nClipPrecision
		DEFAULT_QUALITY, // nQuality
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily
		theApp.m_taskbar_data.font.name);
}

void CTaskBarDlg::ApplySettings()
{
	SetTextFont();
	CalculateWindowWidth();
}

void CTaskBarDlg::CalculateWindowWidth()
{
	bool horizontal_arrange = theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom;
	//计算标签宽度
	m_up_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.up.c_str()).cx;
	m_down_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.down.c_str()).cx;
	m_cpu_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.cpu.c_str()).cx;
	m_memory_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.memory.c_str()).cx;
	//计算显示上传下载部分所需要的宽度
	CString str1, str2;
	int width1, width2;
	CString sample_str;
	wstring digits(theApp.m_taskbar_data.digits_number, L'8');		//根据数据位数生成指定个数的“8”
	bool hide_unit{ theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO };
	if (theApp.m_taskbar_data.speed_short_mode)
	{
		if (hide_unit)
			sample_str.Format(_T("%s."), digits.c_str());
		else
			sample_str.Format(_T("%s.M/s"), digits.c_str());
	}
	else
	{
		if (hide_unit)
			sample_str.Format(_T("%s.8"), digits.c_str());
		else
			sample_str.Format(_T("%s.8MB/s"), digits.c_str());
	}
	if (!hide_unit && theApp.m_taskbar_data.separate_value_unit_with_space)
		sample_str += _T(' ');
	if(theApp.m_taskbar_data.speed_short_mode && !theApp.m_taskbar_data.unit_byte && !theApp.m_taskbar_data.hide_unit)
		sample_str += _T('b');
	str1 = theApp.m_taskbar_data.disp_str.up.c_str() + sample_str;
	str2 = theApp.m_taskbar_data.disp_str.down.c_str() + sample_str;
	width1= m_pDC->GetTextExtent(str1).cx;		//计算使用当前字体显示文本需要的宽度值
	width2= m_pDC->GetTextExtent(str2).cx;
	if (!horizontal_arrange)
		m_window_width_s = max(width1, width2);
	else
		m_window_width_s = width1 + width2 + theApp.DPI(8);
	m_ud_value_width = width1 - m_up_lable_width;

	//计算显示CPU、内存部分所需要的宽度
	int width_cpu_memory;
	if (theApp.m_taskbar_data.hide_percent)
	{
		str1.Format(_T("%s100"), theApp.m_taskbar_data.disp_str.cpu.c_str());
		str2.Format(_T("%s100"), theApp.m_taskbar_data.disp_str.memory.c_str());
	}
	else
	{
		str1.Format(_T("%s100%%"), theApp.m_taskbar_data.disp_str.cpu.c_str());
		str2.Format(_T("%s100%%"), theApp.m_taskbar_data.disp_str.memory.c_str());
	}
	width1 = m_pDC->GetTextExtent(str1).cx;
	width2 = m_pDC->GetTextExtent(str2).cx;
	if (!horizontal_arrange)
		width_cpu_memory = max(width1, width2);
	else
		width_cpu_memory = width1 + width2 + theApp.DPI(4);
	m_window_width = m_window_width_s + width_cpu_memory;
	m_cm_value_width = width1 - m_cpu_lable_width;

	if (!horizontal_arrange)
		m_window_height = TASKBAR_WND_HEIGHT;
	else
		m_window_height = TASKBAR_WND_HEIGHT * 3 / 4;
	m_rect.bottom = m_rect.top + m_window_height;
}

void CTaskBarDlg::SetToolTipsTopMost()
{
	m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CTaskBarDlg::UpdateToolTips()
{
	CString tip_info;
	tip_info = GetMouseTipsInfo();
	m_tool_tips.UpdateTipText(tip_info, this);
}

BOOL CTaskBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//设置隐藏任务栏图标
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	m_pDC = GetDC();

	//设置字体
	SetTextFont();
	m_pDC->SelectObject(&m_font);

	m_window_height = TASKBAR_WND_HEIGHT;
	m_rect.SetRectEmpty();
	m_rect.bottom = m_window_height;

	m_hTaskbar = ::FindWindow(L"Shell_TrayWnd", NULL);		//寻找类名是Shell_TrayWnd的窗口句柄
	m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL);	//寻找二级容器的句柄
	m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);	//寻找最小化窗口的句柄

	::GetWindowRect(m_hMin, m_rcMin);	//获得最小化窗口的区域
	::GetWindowRect(m_hBar, m_rcBar);	//获得二级容器的区域
	m_left_space = m_rcMin.left - m_rcBar.left;
	m_top_space = m_rcMin.top - m_rcBar.top;

	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	CalculateWindowWidth();
	if (!theApp.m_cfg_data.m_tbar_show_cpu_memory)
		m_rect.right = m_rect.left + m_window_width_s;
	else
		m_rect.right = m_rect.left + m_window_width;

	int wnd_x_pos, wnd_y_pos;
	if (m_taskbar_on_top_or_bottom)		//如果任务栏在桌面顶部或底部
	{
		m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//保存最小化窗口宽度

		//通过用MoveWindow函数来改变小化窗口的宽度
		if(!theApp.m_taskbar_data.tbar_wnd_on_left)
			::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
		else
			::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//把程序窗口设置成任务栏的子窗口
		m_error_code = GetLastError();

		//调整程序窗口的大小和位置
		//注：当把当前窗口设置为任务栏的子窗口后，MoveWindow函数移动的位置是基于任务栏的相对位置，
		//在某些情况下，如被安全软件阻止时，窗口无法嵌入任务栏，窗口会移动到基于屏幕左上角的绝对位置。
		if(!theApp.m_taskbar_data.tbar_wnd_on_left)
			wnd_x_pos = m_left_space + m_rcMin.Width() - m_rect.Width() + 2;
		else
			wnd_x_pos = m_left_space;
		wnd_y_pos = (m_rcBar.Height() - m_rect.Height()) / 2;
		if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())		//系统是Win7并且水平排列时，任务栏窗口位置向下调整一个像素
			wnd_y_pos += theApp.DPI(1);
		::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);
	}
	else	//当任务栏在桌面左侧或右侧时
	{
		//设置窗口大小
		int window_width;
		window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
		if (theApp.m_cfg_data.m_tbar_show_cpu_memory)	//将CPU和内存利用率放到网速的下面
		{
			m_rect.right = m_rect.left + window_width;
			m_rect.bottom = m_rect.top + m_rect.Height() * 2 + theApp.DPI(2);
		}
		m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//保存最小化窗口高度

		//通过用MoveWindow函数来改变小化窗口的高度
		if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);
		else
			::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);

		::SetParent(this->m_hWnd, m_hBar);	//把程序窗口设置成任务栏的子窗口
		m_error_code = GetLastError();

		//调整程序窗口的大小和位置
		//注：当把当前窗口设置为任务栏的子窗口后，MoveWindow函数移动的位置是基于任务栏的相对位置，
		//在某些情况下，如被安全软件阻止时，窗口无法嵌入任务栏，窗口会移动到基于屏幕左上角的绝对位置。
		wnd_x_pos = (m_rcMin.Width() - window_width) / 2;
		if (wnd_x_pos < theApp.DPI(2)) wnd_x_pos = theApp.DPI(2);
		if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			wnd_y_pos = m_top_space + m_rcMin.Height() - m_rect.Height() + 2;
		else
			wnd_y_pos = m_top_space;
		::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);

	}
	CRect rect;
	GetWindowRect(rect);	//获取当前窗口的绝对位置
	//如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
	if (rect.left == wnd_x_pos && rect.top == wnd_y_pos)
	{
		rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
		this->MoveWindow(rect);
		m_connot_insert_to_task_bar = true;
		//MessageBox(_T("警告：窗口没有成功嵌入任务栏，可能已被安全软件阻止！"), NULL, MB_ICONWARNING);
	}

	m_menu.LoadMenu(IDR_TASK_BAR_MENU);

	SetBackgroundColor(theApp.m_taskbar_data.back_color);

	//初始化鼠标提示
	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetMaxTipWidth(600);
	m_tool_tips.AddTool(this, _T(""));
	SetToolTipsTopMost();		//设置提示信息总是置顶

	SetTimer(TASKBAR_TIMER, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	//SaveConfig();
	DestroyWindow();
	//程序关闭的时候，把最小化窗口的width恢复回去
	if (IsTaskbarOnTopOrBottom())
		::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width(), m_rcMin.Height(), TRUE);
	else

		::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height(), TRUE);

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


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY2, MF_BYCOMMAND | (theApp.m_cfg_data.m_tbar_show_cpu_memory ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | (theApp.m_cfg_data.m_show_notify_icon ? MF_CHECKED : MF_UNCHECKED));

	pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface ? MF_GRAYED : MF_ENABLED));

	//pMenu->SetDefaultItem(ID_NETWORK_INFO);
	//设置默认菜单项
	switch (theApp.m_taskbar_data.double_click_action)
	{
	case DoubleClickAction::CONNECTION_INFO:
		pMenu->SetDefaultItem(ID_NETWORK_INFO);
		break;
	case DoubleClickAction::HISTORY_TRAFFIC:
		pMenu->SetDefaultItem(ID_TRAFFIC_HISTORY);
		break;
	case DoubleClickAction::SHOW_MORE_INFO:
		pMenu->SetDefaultItem(ID_SHOW_CPU_MEMORY2);
		break;
	case DoubleClickAction::OPTIONS:
		pMenu->SetDefaultItem(ID_OPTIONS2);
		break;
	default:
		pMenu->SetDefaultItem(-1);
		break;
	}
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_MENU_POPED_UP, 0, 0);		//通知主窗口菜单已弹出
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


void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	switch (theApp.m_taskbar_data.double_click_action)
	{
	case DoubleClickAction::CONNECTION_INFO:
		SendMessage(WM_COMMAND,ID_NETWORK_INFO);		//双击后弹出“连接详情”对话框
		break;
	case DoubleClickAction::HISTORY_TRAFFIC:
		SendMessage(WM_COMMAND, ID_TRAFFIC_HISTORY);		//双击后弹出“历史流量统计”对话框
		break;
	case DoubleClickAction::SHOW_MORE_INFO:
		PostMessage(WM_COMMAND, ID_SHOW_CPU_MEMORY2);		//切换显示CPU和内存利用率
		break;
	case DoubleClickAction::OPTIONS:
		SendMessage(WM_COMMAND, ID_OPTIONS2);		//双击后弹出“选项设置”对话框
		break;
	case DoubleClickAction::TASK_MANAGER:
		ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir+L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);		//打开任务管理器
		break;
	default:
		break;
	}
	//CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTaskBarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TASKBAR_TIMER)
	{
		AdjustWindowPos();
		//ShowInfo();
		Invalidate(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CTaskBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	UINT uMsg = LOWORD(wParam);
	if (uMsg == ID_SELECT_ALL_CONNECTION || uMsg == ID_SELETE_CONNECTION
		|| (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELECT_ALL_CONNECTION + 98))
	{
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);	//如果点击了“选择网络连接”子菜单项，将消息转发到主窗口
		return TRUE;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CTaskBarDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()
	ShowInfo(&dc);
}
