// HistoryTrafficDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "HistoryTrafficDlg.h"
#include "afxdialogex.h"


// CHistoryTrafficDlg �Ի���

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
	CCommon::WritePrivateProfileIntW(L"histroy_traffic", L"use_log_scale", m_use_log_scale, theApp.m_config_path.c_str());
}

void CHistoryTrafficDlg::LoadConfig()
{
	m_use_log_scale = (GetPrivateProfileInt(_T("histroy_traffic"), _T("use_log_scale"), 0, theApp.m_config_path.c_str()) != 0);
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


// CHistoryTrafficDlg ��Ϣ�������


BOOL CHistoryTrafficDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetIcon(AfxGetApp()->LoadIcon(IDI_NOFITY_ICON), FALSE);		// ����Сͼ��

	LoadConfig();

	//��ʼ���б�ؼ�
	CRect rect;
	m_history_list.GetClientRect(rect);
	m_history_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	int width0, width1;
	width0 = rect.Width() / 3;
	width1 = rect.Width() - 2 * width0 - theApp.DPI(21);
	m_history_list.InsertColumn(0, _T("����"), LVCFMT_LEFT, width0);		//�����0��
	m_history_list.InsertColumn(1, _T("ʹ�õ��������ϴ�+���أ�"), LVCFMT_LEFT, width0);		//�����1��
	m_history_list.InsertColumn(2, _T("ͼ��"), LVCFMT_LEFT, width1);

	//��ȡ��ʷ�����б������������ֵ
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
		if (m_history_traffics[i].kBytes < 102400)		//����С��100MBʱ������ɫ
			color = RGB(0, 183, 238);
		else if (m_history_traffics[i].kBytes < 1024 * 1024)	//����С��1GBʱ������ɫ
			color = RGB(128, 194, 105);
		else if (m_history_traffics[i].kBytes < 10 * 1024 * 1024)	//����С��10GBʱ���ƻ�ɫ
			color = RGB(255, 216, 58);
		else		//��������10GBʱ���ƺ�ɫ
			color = RGB(255, 95, 74);
		m_history_list.SetDrawItemRangeData(i, range, color);
	}

	//��ȡ��ʼʱ���ڵĴ�С
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	m_menu.LoadMenu(IDR_HISTORY_TRAFFIC_MENU);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CHistoryTrafficDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//���ƴ�����С��С
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//������С���
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//������С�߶�

	CDialog::OnGetMinMaxInfo(lpMMI);
}


BOOL CHistoryTrafficDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �ڴ����ר�ô����/����û���

	LPNMHDR lpnmhdr = (LPNMHDR)lParam;
	if (lpnmhdr->code == NM_RCLICK)
	{
		CPoint point;
		GetCursorPos(&point);//��ù���λ��
		m_history_list.ScreenToClient(&point);//���list�ؼ��ڴ����ϵ�����
		CWnd* pWnd = m_history_list.ChildWindowFromPoint(point);
		CHeaderCtrl* pHeader = m_history_list.GetHeaderCtrl();//��ȡ�б���ͼ�ؼ��ı���ؼ�
		CRect item_rect;
		pHeader->GetItemRect(2, item_rect);		//��ȡ�б����ؼ���2�еľ�������ֻ�й���ڵ�2�е��ʱ�ŵ����Ҽ��˵�
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

	// TODO: �ڴ˴������Ϣ����������
	if (m_use_log_scale)
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LOG_SCALE, MF_BYCOMMAND | MF_CHECKED);
	else
		pMenu->CheckMenuRadioItem(ID_USE_LINEAR_SCALE, ID_USE_LOG_SCALE, ID_USE_LINEAR_SCALE, MF_BYCOMMAND | MF_CHECKED);
}


void CHistoryTrafficDlg::OnUseLinearScale()
{
	// TODO: �ڴ���������������
	m_use_log_scale = false;
	m_history_list.SetDrawItemRangInLogScale(m_use_log_scale);
	SaveConfig();
}


void CHistoryTrafficDlg::OnUseLogScale()
{
	// TODO: �ڴ���������������
	m_use_log_scale = true;
	m_history_list.SetDrawItemRangInLogScale(m_use_log_scale);
	SaveConfig();
}
