// OptionsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg �Ի���

IMPLEMENT_DYNAMIC(COptionsDlg, CDialog)

COptionsDlg::COptionsDlg(int tab, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OPTIONS_DIALOG, pParent), m_tab_selected{ tab }
{

}

COptionsDlg::~COptionsDlg()
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_tab);
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &COptionsDlg::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// COptionsDlg ��Ϣ�������


BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//�����ǩ
	m_tab.InsertItem(0, _T("����������"));
	m_tab.InsertItem(1, _T("��������������"));
	m_tab.InsertItem(2, _T("��������"));
	//�����ӶԻ���
	m_tab1_dlg.Create(IDD_MAIN_WND_SETTINGS_DIALOG, &m_tab);
	m_tab2_dlg.Create(IDD_TASKBAR_SETTINGS_DIALOG, &m_tab);
	m_tab3_dlg.Create(IDD_GENERAL_SETTINGS_DIALOG, &m_tab);
	//�����ӶԻ���Ĵ�С��λ��
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
	m_tab3_dlg.MoveWindow(&rect);
	//����Ĭ��ѡ�еı�ǩ
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

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void COptionsDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_tab_selected = m_tab.GetCurSel();
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.ShowWindow(SW_HIDE);
		m_tab1_dlg.SetFocus();
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.SetFocus();
		break;
	case 2:
		m_tab3_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab3_dlg.SetFocus();
		break;
	}
	*pResult = 0;
}
