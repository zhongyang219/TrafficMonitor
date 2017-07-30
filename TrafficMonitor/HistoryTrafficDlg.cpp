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

void CHistoryTrafficDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORY_INFO_LIST, m_history_list);
}


BEGIN_MESSAGE_MAP(CHistoryTrafficDlg, CDialog)
END_MESSAGE_MAP()


// CHistoryTrafficDlg 消息处理程序


BOOL CHistoryTrafficDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//获取dpi设置
	CWindowDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	int dpi = GetDeviceCaps(hDC, LOGPIXELSY);

	//初始化列表控件
	CRect rect;
	m_history_list.GetClientRect(rect);
	m_history_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	int width0, width1;
	width0 = rect.Width() / 5 * 2;
	width1 = rect.Width() - width0 - 21 * dpi / 96;
	m_history_list.InsertColumn(0, _T("日期"), LVCFMT_LEFT, width0);		//插入第0列
	m_history_list.InsertColumn(1, _T("使用的流量（上传+下载）"), LVCFMT_LEFT, width1);		//插入第1列

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

	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
