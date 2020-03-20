// HistoryTrafficDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "HistoryTrafficDlg.h"
#include "afxdialogex.h"


// CHistoryTrafficDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryTrafficDlg, CDialog)

CHistoryTrafficDlg::CHistoryTrafficDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HISTORY_TRAFFIC_DIALOG, pParent), m_history_traffics(history_traffics), m_tab1_dlg(history_traffics, this), m_tab2_dlg(history_traffics, this)
{

}

CHistoryTrafficDlg::~CHistoryTrafficDlg()
{
    SaveConfig();
}


void CHistoryTrafficDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}

void CHistoryTrafficDlg::SetTabWndSize()
{
	CRect rect;
	m_tab.GetClientRect(rect);
	CRect rcTabItem;
	m_tab.GetItemRect(0, rcTabItem);
	rect.top += rcTabItem.Height() + 4;
	rect.left += 4;
	rect.bottom -= 4;
	rect.right -= 4;
	m_tab1_dlg.MoveWindow(&rect);
	m_tab2_dlg.MoveWindow(&rect);
}

void CHistoryTrafficDlg::LoadConfig()
{
    CIniHelper ini{ theApp.m_config_path };
    m_window_size.cx = ini.GetInt(L"histroy_traffic", L"width", -1);
    m_window_size.cy = ini.GetInt(L"histroy_traffic", L"height", -1);
}

void CHistoryTrafficDlg::SaveConfig() const
{
    CIniHelper ini{ theApp.m_config_path };
    ini.WriteInt(L"histroy_traffic", L"width", m_window_size.cx);
    ini.WriteInt(L"histroy_traffic", L"height", m_window_size.cy);
    ini.Save();
}


BEGIN_MESSAGE_MAP(CHistoryTrafficDlg, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CHistoryTrafficDlg::OnTcnSelchangeTab1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CHistoryTrafficDlg 消息处理程序


BOOL CHistoryTrafficDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(CCommon::LoadText(IDS_TITLE_HISTORY_TRAFFIC));
	SetIcon(AfxGetApp()->LoadIcon(IDI_NOFITY_ICON), FALSE);		// 设置小图标

	//插入标签
	m_tab.InsertItem(0, CCommon::LoadText(IDS_LIST_VIEW));
	m_tab.InsertItem(1, CCommon::LoadText(IDS_CALENDAR_VIEW));
	//创建子对话框
	m_tab1_dlg.Create(IDD_HISTORY_TRAFFIC_LIST_DIALOG, &m_tab);
	m_tab2_dlg.Create(IDD_HISTORY_TRAFFIC_CALENDAR_DIALOG, &m_tab);
	//调整子对话框的大小和位置
	SetTabWndSize();
	//设置默认选中的标签
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		break;
	}
	m_tab.SetCurFocus(m_tab_selected);


	//获取初始时窗口的大小
	CRect rect;
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

    //载入窗口大小设置
    LoadConfig();
    if (m_window_size.cx > 0 && m_window_size.cy > 0)
    {
        SetWindowPos(nullptr, 0, 0, m_window_size.cx, m_window_size.cy, SWP_NOMOVE | SWP_NOZORDER);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CHistoryTrafficDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}




BOOL CHistoryTrafficDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (GetKeyState(VK_CONTROL) & 0x80)
	{
		if (pMsg->wParam == 'D')
		{
			HistoryTraffic h{};
			h.year = 2018;
			h.month = 4;
			h.day = 29;
			auto iter = std::lower_bound(m_history_traffics.begin(), m_history_traffics.end(), h, HistoryTraffic::DateGreater);
			int index = iter - m_history_traffics.begin();
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CHistoryTrafficDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_tab_selected = m_tab.GetCurSel();
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab1_dlg.SetFocus();
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.SetFocus();
		break;
	}
	*pResult = 0;
}


void CHistoryTrafficDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (nType != SIZE_MINIMIZED && m_tab1_dlg.GetSafeHwnd() != NULL && m_tab2_dlg.GetSafeHwnd() != NULL)
	{
		SetTabWndSize();
	}
    if (nType != SIZE_MINIMIZED && nType != SIZE_MAXIMIZED)
    {
        CRect rect;
        GetWindowRect(&rect);
        m_window_size = rect.Size();
    }

}
