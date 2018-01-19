// GeneralSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "GeneralSettingsDlg.h"
#include "afxdialogex.h"


// CGeneralSettingsDlg dialog

IMPLEMENT_DYNAMIC(CGeneralSettingsDlg, CDialogEx)

CGeneralSettingsDlg::CGeneralSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GENERAL_SETTINGS_DIALOG, pParent)
{

}

CGeneralSettingsDlg::~CGeneralSettingsDlg()
{
}

void CGeneralSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGeneralSettingsDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_NOW_BUTTON, &CGeneralSettingsDlg::OnBnClickedCheckNowButton)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CGeneralSettingsDlg::OnBnClickedCheckUpdateCheck)
END_MESSAGE_MAP()


// CGeneralSettingsDlg 消息处理程序


BOOL CGeneralSettingsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetBackgroundColor(RGB(255, 255, 255));

	((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CGeneralSettingsDlg::OnBnClickedCheckNowButton()
{
	// TODO: 在此添加控件通知处理程序代码
	CTrafficMonitorApp::CheckUpdate(true);
}


void CGeneralSettingsDlg::OnBnClickedCheckUpdateCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.check_update_when_start = (((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->GetCheck() != 0);
}
