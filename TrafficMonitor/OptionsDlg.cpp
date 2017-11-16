// OptionsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "OptionsDlg.h"
#include "afxdialogex.h"


// COptionsDlg 对话框

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
	ON_WM_TIMER()
END_MESSAGE_MAP()


// COptionsDlg 消息处理程序


BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//插入标签
	m_tab.InsertItem(0, _T("主窗口设置"));
	m_tab.InsertItem(1, _T("任务栏窗口设置"));
	//创建子对话框
	m_tab1_dlg.Create(IDD_MAIN_WND_SETTINGS_DIALOG, &m_tab);
	m_tab2_dlg.Create(IDD_TASKBAR_SETTINGS_DIALOG, &m_tab);
	//调整子对话框的大小和位置
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
	//设置默认选中的标签
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		SetTimer(TIMER_ID1, 50, NULL);
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		SetTimer(TIMER_ID2, 50, NULL);
		break;
	}
	m_tab.SetCurFocus(m_tab_selected);
	//m_tab.SetCurSel(m_tab_selected);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void COptionsDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	m_tab_selected = m_tab.GetCurSel();
	switch (m_tab_selected)
	{
	case 0:
		m_tab1_dlg.ShowWindow(SW_SHOW);
		m_tab2_dlg.ShowWindow(SW_HIDE);
		m_tab1_dlg.SetFocus();
		SetTimer(TIMER_ID1, 50, NULL);		//点击了第1个选项卡后延迟30毫秒绘制颜色控件
		break;
	case 1:
		m_tab2_dlg.ShowWindow(SW_SHOW);
		m_tab1_dlg.ShowWindow(SW_HIDE);
		m_tab2_dlg.SetFocus();
		SetTimer(TIMER_ID2, 50, NULL);		//点击了第2个选项卡后延迟30毫秒绘制颜色控件
		break;
	}
	*pResult = 0;
}


void COptionsDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == TIMER_ID1)
	{
		m_tab1_dlg.DrawStaticColor();
		KillTimer(TIMER_ID1);
	}

	if (nIDEvent == TIMER_ID2)
	{
		m_tab2_dlg.DrawStaticColor();
		KillTimer(TIMER_ID2);
	}

	CDialog::OnTimer(nIDEvent);
}
