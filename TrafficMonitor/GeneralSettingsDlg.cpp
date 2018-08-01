// GeneralSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "GeneralSettingsDlg.h"
#include "afxdialogex.h"


// CGeneralSettingsDlg dialog

IMPLEMENT_DYNAMIC(CGeneralSettingsDlg, CTabDlg)

CGeneralSettingsDlg::CGeneralSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_GENERAL_SETTINGS_DIALOG, pParent)
{

}

CGeneralSettingsDlg::~CGeneralSettingsDlg()
{
}

void CGeneralSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_EDIT, m_traffic_tip_edit);
	DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_COMBO, m_traffic_tip_combo);
	DDX_Control(pDX, IDC_MEMORY_USAGE_TIP_EDIT, m_memory_tip_edit);
	DDX_Control(pDX, IDC_LANGUAGE_COMBO, m_language_combo);
}

void CGeneralSettingsDlg::SetTrafficTipControlEnable(bool enable)
{
	m_traffic_tip_edit.EnableWindow(enable);
	m_traffic_tip_combo.EnableWindow(enable);
}

void CGeneralSettingsDlg::SetMemoryTipControlEnable(bool enable)
{
	m_memory_tip_edit.EnableWindow(enable);
}


BEGIN_MESSAGE_MAP(CGeneralSettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_CHECK_NOW_BUTTON, &CGeneralSettingsDlg::OnBnClickedCheckNowButton)
	ON_BN_CLICKED(IDC_CHECK_UPDATE_CHECK, &CGeneralSettingsDlg::OnBnClickedCheckUpdateCheck)
	ON_BN_CLICKED(IDC_AUTO_RUN_CHECK, &CGeneralSettingsDlg::OnBnClickedAutoRunCheck)
	ON_BN_CLICKED(IDC_ALLOW_SKIN_FONT_CHECK, &CGeneralSettingsDlg::OnBnClickedAllowSkinFontCheck)
	ON_BN_CLICKED(IDC_ALLOW_SKIN_DISP_STR_CHECK, &CGeneralSettingsDlg::OnBnClickedAllowSkinDispStrCheck)
	ON_BN_CLICKED(IDC_TODAY_TRAFFIC_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedTodayTrafficTipCheck)
	ON_BN_CLICKED(IDC_MEMORY_USAGE_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedMemoryUsageTipCheck)
	ON_BN_CLICKED(IDC_OPEN_CONFIG_PATH_BUTTON, &CGeneralSettingsDlg::OnBnClickedOpenConfigPathButton)
	ON_BN_CLICKED(IDC_SHOW_ALL_CONNECTION_CHECK, &CGeneralSettingsDlg::OnBnClickedShowAllConnectionCheck)
END_MESSAGE_MAP()


// CGeneralSettingsDlg 消息处理程序


BOOL CGeneralSettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
	((CButton*)GetDlgItem(IDC_ALLOW_SKIN_FONT_CHECK))->SetCheck(m_data.allow_skin_cover_font);
	((CButton*)GetDlgItem(IDC_ALLOW_SKIN_DISP_STR_CHECK))->SetCheck(m_data.allow_skin_cover_text);

	//检查开始菜单的“启动”目录下有没有程序的快捷方式，如果有则设置开机自启动，然后删除快捷方式
	wstring start_up_path = CCommon::GetStartUpPath();
	bool shortcut_exist = CCommon::FileExist((start_up_path + L"\\TrafficMonitor.lnk").c_str());
	if (shortcut_exist)
	{
		theApp.SetAutoRun(true);
		m_data.auto_run = true;
		DeleteFile((start_up_path + L"\\TrafficMonitor.lnk").c_str());
	}
	else
	{
		m_data.auto_run = theApp.GetAutoRun();
	}
	((CButton*)GetDlgItem(IDC_AUTO_RUN_CHECK))->SetCheck(m_data.auto_run);

	((CButton*)GetDlgItem(IDC_TODAY_TRAFFIC_TIP_CHECK))->SetCheck(m_data.traffic_tip_enable);
	m_traffic_tip_edit.SetRange(1, 32767);
	m_traffic_tip_edit.SetValue(m_data.traffic_tip_value);
	m_traffic_tip_combo.AddString(_T("MB"));
	m_traffic_tip_combo.AddString(_T("GB"));
	m_traffic_tip_combo.SetCurSel(m_data.traffic_tip_unit);
	((CButton*)GetDlgItem(IDC_MEMORY_USAGE_TIP_CHECK))->SetCheck(m_data.memory_usage_tip_enable);
	m_memory_tip_edit.SetRange(1, 100);
	m_memory_tip_edit.SetValue(m_data.memory_tip_value);
	SetTrafficTipControlEnable(m_data.traffic_tip_enable);
	SetMemoryTipControlEnable(m_data.memory_usage_tip_enable);

	m_language_combo.AddString(CCommon::LoadText(IDS_FOLLOWING_SYSTEM));
	m_language_combo.AddString(_T("English"));
	m_language_combo.AddString(_T("简体中文"));
	m_language_combo.AddString(_T("繁體中文"));
	m_language_combo.SetCurSel(static_cast<int>(m_data.language));

	((CButton*)GetDlgItem(IDC_SHOW_ALL_CONNECTION_CHECK))->SetCheck(m_data.show_all_interface);

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_SHOW_ALL_CONNECTION_CHECK), CCommon::LoadText(IDS_SHOW_ALL_INFO_TIP));

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


void CGeneralSettingsDlg::OnBnClickedAutoRunCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.auto_run = (((CButton*)GetDlgItem(IDC_AUTO_RUN_CHECK))->GetCheck() != 0);
	m_auto_run_modified = true;
}


void CGeneralSettingsDlg::OnBnClickedAllowSkinFontCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.allow_skin_cover_font = (((CButton*)GetDlgItem(IDC_ALLOW_SKIN_FONT_CHECK))->GetCheck() != 0);
}


void CGeneralSettingsDlg::OnBnClickedAllowSkinDispStrCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.allow_skin_cover_text = (((CButton*)GetDlgItem(IDC_ALLOW_SKIN_DISP_STR_CHECK))->GetCheck() != 0);
}


void CGeneralSettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	//获取消息提示的设置
	m_data.traffic_tip_value = m_traffic_tip_edit.GetValue();
	if (m_data.traffic_tip_value < 1) m_data.traffic_tip_value = 1;
	if (m_data.traffic_tip_value > 32767) m_data.traffic_tip_value = 32767;
	m_data.traffic_tip_unit = m_traffic_tip_combo.GetCurSel();
	m_data.memory_tip_value = m_memory_tip_edit.GetValue();
	if (m_data.memory_tip_value < 1) m_data.memory_tip_value = 1;
	if (m_data.memory_tip_value > 100) m_data.memory_tip_value = 100;
	//获取语言的设置
	m_data.language = static_cast<Language>(m_language_combo.GetCurSel());
	if (m_data.language != theApp.m_general_data.language)
	{
		MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);;
	}
	m_show_all_interface_modified = (m_data.show_all_interface != theApp.m_general_data.show_all_interface);

	CTabDlg::OnOK();
}


void CGeneralSettingsDlg::OnBnClickedTodayTrafficTipCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.traffic_tip_enable = (((CButton*)GetDlgItem(IDC_TODAY_TRAFFIC_TIP_CHECK))->GetCheck() != 0);
	SetTrafficTipControlEnable(m_data.traffic_tip_enable);
}


void CGeneralSettingsDlg::OnBnClickedMemoryUsageTipCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.memory_usage_tip_enable = (((CButton*)GetDlgItem(IDC_MEMORY_USAGE_TIP_CHECK))->GetCheck() != 0);
	SetMemoryTipControlEnable(m_data.memory_usage_tip_enable);
}


void CGeneralSettingsDlg::OnBnClickedOpenConfigPathButton()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(NULL, _T("explore"), theApp.m_config_dir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}


void CGeneralSettingsDlg::OnBnClickedShowAllConnectionCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.show_all_interface = (((CButton*)GetDlgItem(IDC_SHOW_ALL_CONNECTION_CHECK))->GetCheck() != 0);
}


BOOL CGeneralSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_toolTip.RelayEvent(pMsg);

	return CTabDlg::PreTranslateMessage(pMsg);
}
