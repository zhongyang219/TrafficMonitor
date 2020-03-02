// TaskBarDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"


// CTaskBarDlg �Ի���

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


// CTaskBarDlg ��Ϣ�������


void CTaskBarDlg::ShowInfo(CDC* pDC)
{
	if (this->m_hWnd == NULL || pDC == nullptr) return;
	CString str;
	CString in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_taskbar_data);
	CString out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_taskbar_data);

	if (m_rect.IsRectEmpty() || m_rect.IsRectNull()) return;

	//����Ҫ���Ƶ��ı���ɫ
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

	//���û����DC
	CDC MemDC;
	CBitmap MemBitmap;
	MemDC.CreateCompatibleDC(NULL);
	int draw_width = (theApp.m_cfg_data.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
	MemBitmap.CreateCompatibleBitmap(pDC, draw_width, m_rect.Height());
	MemDC.SelectObject(&MemBitmap);
	//��ͼ
	CRect value_rect{ m_rect };		//��ʾ��ֵ�ľ�������
	CRect lable_rect;				//��ʾ�ı���ǩ�ľ�������
	value_rect.MoveToXY(0, 0);
	MemDC.FillSolidRect(value_rect, theApp.m_taskbar_data.back_color);		//��䱳��ɫ
	CDrawCommon draw;
	draw.Create(&MemDC, nullptr);
	draw.SetFont(&m_font);
	draw.SetBackColor(theApp.m_taskbar_data.back_color);
	Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? Alignment::RIGHT : Alignment::LEFT };		//��ֵ�Ķ��뷽ʽ
	//�����ϴ��ٶ�
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
			value_rect.right = value_rect.left + ((m_rect.Width() - value_rect.left) > window_width ? window_width : (m_rect.Width() - value_rect.left)) - theApp.DPI(1);
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
	//�����ϴ���ǩ
	if (!theApp.m_taskbar_data.swap_up_down)
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.up.c_str(), text_colors[0], Alignment::LEFT, true);
	else
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.down.c_str(), text_colors[2], Alignment::LEFT, true);
	//���������ٶ�
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
	//�������ر�ǩ
	if (!theApp.m_taskbar_data.swap_up_down)
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.down.c_str(), text_colors[2], Alignment::LEFT, true);
	else
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.up.c_str(), text_colors[0], Alignment::LEFT, true);

	if (theApp.m_cfg_data.m_tbar_show_cpu_memory)
	{
		//����CPU������
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
			value_rect.right = value_rect.left + ((m_rect.Width() - value_rect.left) > window_width ? window_width : (m_rect.Width() - value_rect.left)) - theApp.DPI(1);
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
		if (theApp.m_cpu_usage == 100)		//���CPU����Ϊ100%���򽫾��ο��Ҳ�����һЩ����ֹ��ʾ��ȫ
			rect_tmp.right += theApp.DPI(5);

		// ����״̬��
		TryDrawStatusBar(draw, CRect(lable_rect.TopLeft(), rect_tmp.BottomRight()), theApp.m_cpu_usage);
		// �����ı�
		draw.DrawWindowText(rect_tmp, str, text_colors[5], value_alignment, false);		//������ֵ
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.cpu.c_str(), text_colors[4], Alignment::LEFT, false);				//���Ʊ�ǩ

		//�����ڴ�������
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
		if (theApp.m_memory_usage == 100)		//����ڴ�����Ϊ100%���򽫾��ο��Ҳ�����һЩ����ֹ��ʾ��ȫ
			rect_tmp.right += theApp.DPI(5);

		// ����״̬��
		TryDrawStatusBar(draw, CRect(lable_rect.TopLeft(), rect_tmp.BottomRight()), theApp.m_memory_usage);
		// �����ı�
		draw.DrawWindowText(rect_tmp, str, text_colors[7], value_alignment, false);
		draw.DrawWindowText(lable_rect, theApp.m_taskbar_data.disp_str.memory.c_str(), text_colors[6], Alignment::LEFT, false);

	}
	//��������DC�е�ͼ�񿽱�����Ļ����ʾ
	pDC->BitBlt(0,0, draw_width, m_rect.Height(), &MemDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
}

void CTaskBarDlg::TryDrawStatusBar(CDrawCommon& drawer, const CRect& rect_bar, int usage_percent)
{
	if (!theApp.m_taskbar_data.show_status_bar)
	{
		return;
	}

	CSize fill_size = CSize(rect_bar.Width() * usage_percent / 100, rect_bar.Height());
	CRect rect_fill(rect_bar.TopLeft(), fill_size);
	drawer.DrawRectOutLine(rect_bar, theApp.m_taskbar_data.status_bar_color, 1, true);
	drawer.FillRect(rect_fill, theApp.m_taskbar_data.status_bar_color);
}

bool CTaskBarDlg::AdjustWindowPos()
{
	if (this->m_hWnd == NULL)
		return false;
	CRect rcMin, rcBar;
	::GetWindowRect(m_hMin, rcMin);	//�����С�����ڵ�����
	::GetWindowRect(m_hBar, rcBar);	//��ö�������������
	static bool last_taskbar_on_top_or_bottom;
	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	if (m_taskbar_on_top_or_bottom != last_taskbar_on_top_or_bottom)
	{
		CalculateWindowWidth();
		last_taskbar_on_top_or_bottom = m_taskbar_on_top_or_bottom;
	}

	if (m_taskbar_on_top_or_bottom)		//�������������涥����ײ�ʱ
	{
		//���ô��ڴ�С
		m_rect.right = m_rect.left + (theApp.m_cfg_data.m_tbar_show_cpu_memory ? m_window_width : m_window_width_s);
		m_rect.bottom = m_rect.top + m_window_height;
		if (rcMin.Width() != m_min_bar_width)	//�����С�����ڵĿ�ȸı��ˣ������������������ڵ�λ��
		{
			m_left_space = rcMin.left - rcBar.left;
			m_rcMin = rcMin;
			m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//������С�����ڿ��
			if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			{
				::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);	//������С�����ڵ�λ��
				m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
			}
			else
			{
				::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
				m_rect.MoveToX(m_left_space);
			}
			m_rect.MoveToY((rcBar.Height() - m_rect.Height()) / 2);
			if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
				m_rect.MoveToY(m_rect.top + theApp.DPI(1));
			MoveWindow(m_rect);
		}
	}
	else		//������������Ļ�������Ҳ�ʱ
	{
		//���ô��ڴ�С
		int window_width;
		window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
		m_rect.right = m_rect.left + ((rcMin.Width() - m_rect.left) > window_width ? window_width : (rcMin.Width() - m_rect.left));
		m_rect.bottom = m_rect.top + (theApp.m_cfg_data.m_tbar_show_cpu_memory ? (2 * m_window_height + theApp.DPI(2)) : m_window_height);
		if (rcMin.Height() != m_min_bar_height)	//�����С�����ڵĸ߶ȸı��ˣ������������������ڵ�λ��
		{
			m_top_space = rcMin.top - rcBar.top;
			m_rcMin = rcMin;
			m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//������С�����ڸ߶�
			if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			{
				::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//������С�����ڵ�λ��
				m_rect.MoveToY(m_top_space + m_rcMin.Height() - m_rect.Height() + 2);
			}
			else
			{
				::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);	//������С�����ڵ�λ��
				m_rect.MoveToY(m_top_space);
			}
			m_rect.MoveToX((m_rcMin.Width() - window_width) / 2);
			if (m_rect.left < theApp.DPI(2))
				m_rect.MoveToX(theApp.DPI(2));
			MoveWindow(m_rect);
		}
	}

	CRect rect{ m_rect };
	//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
	if (m_connot_insert_to_task_bar)
	{
		rect.MoveToXY(rect.left + rcBar.left, rect.top + rcBar.top);
		this->MoveWindow(rect);
	}

	if (m_connot_insert_to_task_bar && ::GetForegroundWindow() == m_hTaskbar)	//�ڴ����޷�Ƕ��������ʱ������������������������ϣ����ô����ö�
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);			//�����ö�
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
		::GetWindowRect(m_hMin, rcMin);	//�����С�����ڵ�����
		::GetWindowRect(m_hBar, rcBar);	//��ö�������������
		if(m_left_space==0)
			m_left_space = rcMin.left - rcBar.left;
		if(m_top_space==0)
			m_top_space = rcMin.top - rcBar.top;

		::GetWindowRect(m_hTaskbar, rect);			//��ȡ�������ľ�������
		return (rect.Width()>=rect.Height());		//����������Ŀ�ȴ��ڸ߶ȣ�����������Ļ�Ķ�����ײ�
	}
	else
	{
		return true;
	}
}

CString CTaskBarDlg::GetMouseTipsInfo()
{
	CString tip_info;
	CString temp;
	temp.Format(_T("%s: %s (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
		CCommon::KBytesToString(static_cast<unsigned int>((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024)),
		CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_up_traffic / 1024)),
		CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(static_cast<unsigned int>(theApp.m_today_down_traffic / 1024))
	);
	tip_info += temp;
	if (theApp.m_cfg_data.m_tbar_show_cpu_memory)
	{
		temp.Format(_T("\r\n%s: %s/%s"),
			CCommon::LoadText(IDS_MEMORY_USAGE),
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
	}
	else
	{
		temp.Format(_T("\r\n%s: %d%%\r\n%s: %s/%s (%d%%)"),
			CCommon::LoadText(IDS_CPU_USAGE),
			theApp.m_cpu_usage,
			CCommon::LoadText(IDS_MEMORY_USAGE),
			CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory),
			theApp.m_memory_usage);
	}
	tip_info += temp;
	return tip_info;
}

void CTaskBarDlg::SetTextFont()
{
	//���m_font�Ѿ�������һ��������Դ�������ͷ���
	if (m_font.m_hObject)
	{
		m_font.DeleteObject();
	}
	//�����µ�����
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
	//�����ǩ���
	m_up_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.up.c_str()).cx;
	m_down_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.down.c_str()).cx;
	m_cpu_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.cpu.c_str()).cx;
	m_memory_lable_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.memory.c_str()).cx;
	//������ʾ�ϴ����ز�������Ҫ�Ŀ��
	CString str1, str2;
	int width1, width2;
	CString sample_str;
	wstring digits(theApp.m_taskbar_data.digits_number, L'8');		//��������λ������ָ�������ġ�8��
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
	width1= m_pDC->GetTextExtent(str1).cx;		//����ʹ�õ�ǰ������ʾ�ı���Ҫ�Ŀ��ֵ
	width2= m_pDC->GetTextExtent(str2).cx;
	if (!horizontal_arrange)
		m_window_width_s = max(width1, width2);
	else
		m_window_width_s = width1 + width2 + theApp.DPI(8);
	m_ud_value_width = width1 - m_up_lable_width;

	//������ʾCPU���ڴ沿������Ҫ�Ŀ��
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
	if (theApp.m_taskbar_data.show_tool_tip)
	{
		CString tip_info;
		tip_info = GetMouseTipsInfo();
		m_tool_tips.UpdateTipText(tip_info, this);
	}
}

BOOL CTaskBarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//��������������ͼ��
	ModifyStyleEx(0, WS_EX_TOOLWINDOW);

	m_pDC = GetDC();


	//��������
	SetTextFont();
	m_pDC->SelectObject(&m_font);

	m_window_height = TASKBAR_WND_HEIGHT;
	m_rect.SetRectEmpty();
	m_rect.bottom = m_window_height;

	m_hTaskbar = ::FindWindow(L"Shell_TrayWnd", NULL);		//Ѱ��������Shell_TrayWnd�Ĵ��ھ��
	m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL);	//Ѱ�Ҷ��������ľ��
	m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);	//Ѱ����С�����ڵľ��

	//���ô���͸��ɫ
#ifndef COMPILE_FOR_WINXP
	if(theApp.m_taskbar_data.transparent_color != 0 && theApp.m_taksbar_transparent_color_enable)
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(theApp.m_taskbar_data.transparent_color, 0, LWA_COLORKEY);
	}
#endif // !COMPILE_FOR_WINXP

	::GetWindowRect(m_hMin, m_rcMin);	//�����С�����ڵ�����
	::GetWindowRect(m_hBar, m_rcBar);	//��ö�������������
	m_left_space = m_rcMin.left - m_rcBar.left;
	m_top_space = m_rcMin.top - m_rcBar.top;

	m_taskbar_on_top_or_bottom = IsTaskbarOnTopOrBottom();
	CalculateWindowWidth();
	if (!theApp.m_cfg_data.m_tbar_show_cpu_memory)
		m_rect.right = m_rect.left + m_window_width_s;
	else
		m_rect.right = m_rect.left + m_window_width;

	if (m_taskbar_on_top_or_bottom)		//��������������涥����ײ�
	{
		m_min_bar_width = m_rcMin.Width() - m_rect.Width();	//������С�����ڿ��

		//ͨ����MoveWindow�������ı�С�����ڵĿ��
		if(!theApp.m_taskbar_data.tbar_wnd_on_left)
			::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
		else
			::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);

		m_connot_insert_to_task_bar = !(::SetParent(this->m_hWnd, m_hBar));	//�ѳ��򴰿����ó����������Ӵ���
		//m_connot_insert_to_task_bar = true;
		m_error_code = GetLastError();

		//�������򴰿ڵĴ�С��λ��
		if(!theApp.m_taskbar_data.tbar_wnd_on_left)
			m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
		else
			m_rect.MoveToX(m_left_space);
		m_rect.MoveToY((m_rcBar.Height() - m_rect.Height()) / 2);
		if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())		//ϵͳ��Win7����ˮƽ����ʱ������������λ�����µ���һ������
			m_rect.MoveToY(m_rect.top + theApp.DPI(1));
		//::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);
		MoveWindow(m_rect);
	}
	else	//�������������������Ҳ�ʱ
	{
		//���ô��ڴ�С
		int window_width;
		window_width = max(m_window_width_s, (m_window_width - m_window_width_s));
		if (theApp.m_cfg_data.m_tbar_show_cpu_memory)	//��CPU���ڴ������ʷŵ����ٵ�����
		{
			m_rect.right = m_rect.left + window_width;
			m_rect.bottom = m_rect.top + m_rect.Height() * 2 + theApp.DPI(2);
		}
		m_min_bar_height = m_rcMin.Height() - m_rect.Height();	//������С�����ڸ߶�

		//ͨ����MoveWindow�������ı�С�����ڵĸ߶�
		if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);
		else
			::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);

		m_connot_insert_to_task_bar = (!::SetParent(this->m_hWnd, m_hBar));	//�ѳ��򴰿����ó����������Ӵ���
		//m_connot_insert_to_task_bar = true;
		m_error_code = GetLastError();

		//�������򴰿ڵĴ�С��λ��
		m_rect.MoveToX((m_rcMin.Width() - window_width) / 2);
		if (m_rect.left < theApp.DPI(2))
			m_rect.MoveToX(theApp.DPI(2));
		if (!theApp.m_taskbar_data.tbar_wnd_on_left)
			m_rect.MoveToY(m_top_space + m_rcMin.Height() - m_rect.Height() + 2);
		else
			m_rect.MoveToY(m_top_space);
		MoveWindow(m_rect);
	}
	CRect rect{ m_rect };
	//�������û�б��ɹ�Ƕ�뵽�������������ƶ����˻�����Ļ���Ͻǵľ���λ�ã����������ڵ�λ��
	if (m_connot_insert_to_task_bar)
	{
		rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
		this->MoveWindow(rect);
	}

	m_menu.LoadMenu(IDR_TASK_BAR_MENU);

	SetBackgroundColor(theApp.m_taskbar_data.back_color);

	//��ʼ�������ʾ
	m_tool_tips.Create(this, TTS_ALWAYSTIP);
	m_tool_tips.SetMaxTipWidth(600);
	m_tool_tips.AddTool(this, _T(""));
	SetToolTipsTopMost();		//������ʾ��Ϣ�����ö�

	//SetTimer(TASKBAR_TIMER, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CTaskBarDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	//SaveConfig();
	DestroyWindow();
	//����رյ�ʱ�򣬰���С�����ڵ�width�ָ���ȥ
	if (IsTaskbarOnTopOrBottom())
		::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width(), m_rcMin.Height(), TRUE);
	else

		::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height(), TRUE);

	//CDialogEx::OnCancel();
}


void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CPoint point1;	//����һ������ȷ�����λ�õ�λ��  
	GetCursorPos(&point1);	//��ȡ��ǰ����λ�ã��Ա�ʹ�ò˵����Ը�����  
	m_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //��ָ��λ����ʾ�����˵�
	CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
	CDialogEx::OnInitMenu(pMenu);

	// TODO: �ڴ˴������Ϣ����������
	pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY2, MF_BYCOMMAND | (theApp.m_cfg_data.m_tbar_show_cpu_memory ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
	pMenu->CheckMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | (theApp.m_cfg_data.m_show_notify_icon ? MF_CHECKED : MF_UNCHECKED));

	pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface ? MF_GRAYED : MF_ENABLED));

	//pMenu->SetDefaultItem(ID_NETWORK_INFO);
	//����Ĭ�ϲ˵���
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
	::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_MENU_POPED_UP, 0, 0);		//֪ͨ�����ڲ˵��ѵ���
}


BOOL CTaskBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//���ΰ��س�����ESC���˳�
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

	if (theApp.m_taskbar_data.show_tool_tip && m_tool_tips.GetSafeHwnd() && (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_LBUTTONUP ||
		pMsg->message == WM_MOUSEMOVE))
	{
		m_tool_tips.RelayEvent(pMsg);
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialogEx::OnMouseMove(nFlags, point);
}


void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (theApp.m_taskbar_data.double_click_action)
	{
	case DoubleClickAction::CONNECTION_INFO:
		SendMessage(WM_COMMAND,ID_NETWORK_INFO);		//˫���󵯳����������顱�Ի���
		break;
	case DoubleClickAction::HISTORY_TRAFFIC:
		SendMessage(WM_COMMAND, ID_TRAFFIC_HISTORY);		//˫���󵯳�����ʷ����ͳ�ơ��Ի���
		break;
	case DoubleClickAction::SHOW_MORE_INFO:
		PostMessage(WM_COMMAND, ID_SHOW_CPU_MEMORY2);		//�л���ʾCPU���ڴ�������
		break;
	case DoubleClickAction::OPTIONS:
		SendMessage(WM_COMMAND, ID_OPTIONS2);		//˫���󵯳���ѡ�����á��Ի���
		break;
	case DoubleClickAction::TASK_MANAGER:
		ShellExecuteW(NULL, _T("open"), (theApp.m_taskbar_data.double_click_exe).c_str(), NULL, NULL, SW_NORMAL);	//��ָ������Ĭ�����������
		break;
	default:
		break;
	}
	//CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTaskBarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//if (nIDEvent == TASKBAR_TIMER)
	//{
	//	AdjustWindowPos();
	//	//ShowInfo();
	//	Invalidate(FALSE);
	//}

	CDialogEx::OnTimer(nIDEvent);
}


BOOL CTaskBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ����ר�ô����/����û���
	UINT uMsg = LOWORD(wParam);
	if (uMsg == ID_SELECT_ALL_CONNECTION || uMsg == ID_SELETE_CONNECTION
		|| (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELECT_ALL_CONNECTION + 98))
	{
		::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);	//�������ˡ�ѡ���������ӡ��Ӳ˵������Ϣת����������
		return TRUE;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CTaskBarDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �ڴ˴������Ϣ����������
					   // ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
	ShowInfo(&dc);
}
