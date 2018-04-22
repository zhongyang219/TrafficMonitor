// HistoryTrafficDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "HistoryTrafficDlg.h"
#include "afxdialogex.h"


// CHistoryTrafficDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryTrafficDlg, CDialog)

CHistoryTrafficDlg::CHistoryTrafficDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HISTORY_TRAFFIC_DIALOG, pParent), m_history_traffics(history_traffics)
{

}

CHistoryTrafficDlg::~CHistoryTrafficDlg()
{
}

void CHistoryTrafficDlg::SaveConfig() const
{
	CIniHelper ini;
	ini.SetPath(theApp.m_config_path);
	ini.WriteBool(L"histroy_traffic", L"use_log_scale", m_use_log_scale);
}

void CHistoryTrafficDlg::LoadConfig()
{
	CIniHelper ini;
	ini.SetPath(theApp.m_config_path);
	m_use_log_scale = ini.GetBool(_T("histroy_traffic"), _T("use_log_scale"), false);
}

void CHistoryTrafficDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_INFO_LIST, m_history_list);
}


BEGIN_MESSAGE_MAP(CHistoryTrafficDlg, CDialog)
	ON_WM_GETMINMAXINFO()
	ON_WM_INITMENU()
	ON_COMMAND(ID_USE_LINEAR_SCALE, &CHistoryTrafficDlg::OnUseLinearScale)
	ON_COMMAND(ID_USE_LOG_SCALE, &CHistoryTrafficDlg::OnUseLogScale)
END_MESSAGE_MAP()


// CHistoryTrafficDlg 消息处理程序


BOOL CHistoryTrafficDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDI_NOFITY_ICON), FALSE);		// 设置小图标

	LoadConfig();

	//初始化列表控件
	CRect rect;
	m_history_list.GetClientRect(rect);
	m_history_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	int width0, width1;
	width0 = rect.Width() / 3;
	width1 = rect.Width() - 2 * width0 - theApp.DPI(21);
	m_history_list.InsertColumn(0, CCommon::LoadText(IDS_DATE), LVCFMT_LEFT, width0);		//插入第0列
	m_history_list.InsertColumn(1, CCommon::LoadText(IDS_TRAFFIC_USED), LVCFMT_LEFT, width0);		//插入第1列
	m_history_list.InsertColumn(2, CCommon::LoadText(IDS_FIGURE), LVCFMT_LEFT, width1);

	//获取历史流量列表中流量的最大值
	unsigned int max_traffic{};
	for (const auto& traffic : m_history_traffics)
	{
		if (traffic.kBytes > max_traffic)
			max_traffic = traffic.kBytes;
	}
	m_history_list.EnableDrawItemRange();
	m_history_list.SetDrawItemRangeRow(2);
	m_history_list.SetDrawItemRangMargin(theApp.DPI(4));
	m_history_list.SetDrawItemRangInLogScale(m_use_log_scale);

	for (int i{}; i<m_history_traffics.size(); i++)
	{
		CString date_str;
		CString k_bytes_str;
		date_str.Format(_T("%.4d/%.2d/%.2d"), m_history_traffics[i].year, m_history_traffics[i].month, m_history_traffics[i].day);

		if (m_history_traffics[i].kBytes < 1024)
			k_bytes_str.Format(_T("%dKB"), m_history_traffics[i].kBytes);
		else if (m_history_traffics[i].kBytes < 1024*1024)
			k_bytes_str.Format(_T("%.2fMB"), m_history_traffics[i].kBytes / 1024.0);
		else
			k_bytes_str.Format(_T("%.2fGB"), m_history_traffics[i].kBytes / 1024.0 / 1024.0);

		m_history_list.InsertItem(i, date_str);
		m_history_list.SetItemText(i, 1, k_bytes_str);
		double range = static_cast<double>(m_history_traffics[i].kBytes) *1000 / max_traffic;
		COLORREF color;
		if (m_history_traffics[i].kBytes < 102400)		//流量小于100MB时绘制蓝色
			color = RGB(0, 183, 238);
		else if (m_history_traffics[i].kBytes < 1024 * 1024)	//流量小于1GB时绘制绿色
			color = RGB(128, 194, 105);
		else if (m_history_traffics[i].kBytes < 10 * 1024 * 1024)	//流量小于10GB时绘制黄色
			color = RGB(255, 216, 58);
		else		//流量大于10GB时绘制红色
			color = RGB(255, 95, 74);
		m_history_list.SetDrawItemRangeData(i, range, color);
	}

	//获取初始时窗口的大小
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	m_menu.LoadMenu(IDR_HISTORY_TRAFFIC_MENU);

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


BOOL CHistoryTrafficDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: 在此添加专用代码和/或调用基类

	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	if (lpnmhdr->code == NM_RCLICK)
	{
		CPoint point;
		GetCursorPos(&point);//获得光标的位置
		m_history_list.ScreenToClient(&point);//获得list控件在窗口上的坐标
		CWnd* pWnd = m_history_list.ChildWindowFromPoint(point);
		CHeaderCtrl* pHeader = m_history_list.GetHeaderCtrl();//获取列表视图控件的标题控件
		CRect item_rect;
		pHeader->GetItemRect(2, item_rect);		//获取列表标题控件第2列的矩形区域，只有光标在第2列点击时才弹出右键菜单
		if (pWnd && (pWnd->GetSafeHwnd() == pHeader->GetSafeHwnd()) && item_rect.PtInRect(point))
		{
			HDHITTESTINFO info{};
			info.pt = point;
			pHeader->SendMessage(HDM_HITTEST, 0, (LPARAM)&info);
			CMenu * pMenu = m_menu.GetSubMenu(0);
			GetCursorPos(&point);
			pMenu->TrackPopupMenu(TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}


void CHistoryTrafficDlg::OnInitMenu(CMenu* pMenu)
{
	CDialog::OnInitMenu(pMenu);

	// TODO: 在此处添加消息处理程序代码
	if (m_use_log_scale)
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LOG_SCALE, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LINEAR_SCALE, MF_BYCOMMAND | MF_CHECKED);
}


void CHistoryTrafficDlg::OnUseLinearScale()
{
	// TODO: 在此添加命令处理程序代码
	m_use_log_scale = false;
	m_history_list.SetDrawItemRangInLogScale(m_use_log_scale);
	SaveConfig();
}


void CHistoryTrafficDlg::OnUseLogScale()
{
	// TODO: 在此添加命令处理程序代码
	m_use_log_scale = true;
	m_history_list.SetDrawItemRangInLogScale(m_use_log_scale);
	SaveConfig();
}
