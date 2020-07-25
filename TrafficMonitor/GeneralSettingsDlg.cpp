// GeneralSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "GeneralSettingsDlg.h"
#include "afxdialogex.h"


// CGeneralSettingsDlg dialog

static const int MONITOR_SPAN_STEP = 100;

IMPLEMENT_DYNAMIC(CGeneralSettingsDlg, CTabDlg)

CGeneralSettingsDlg::CGeneralSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_GENERAL_SETTINGS_DIALOG, pParent)
{

}

CGeneralSettingsDlg::~CGeneralSettingsDlg()
{
}

bool CGeneralSettingsDlg::IsMonitorTimeSpanModified() const
{
    return m_data.monitor_time_span != m_monitor_time_span_ori;
}

void CGeneralSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_EDIT, m_traffic_tip_edit);
    DDX_Control(pDX, IDC_TODAY_TRAFFIC_TIP_COMBO, m_traffic_tip_combo);
    DDX_Control(pDX, IDC_MEMORY_USAGE_TIP_EDIT, m_memory_tip_edit);
    DDX_Control(pDX, IDC_LANGUAGE_COMBO, m_language_combo);
    DDX_Control(pDX, IDC_MONITOR_SPAN_EDIT, m_monitor_span_edit);
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
	ON_BN_CLICKED(IDC_USE_CPU_TIME_RADIO, &CGeneralSettingsDlg::OnBnClickedUseCpuTimeRadio)
	ON_BN_CLICKED(IDC_USE_PDH_RADIO, &CGeneralSettingsDlg::OnBnClickedUsePdhRadio)
    ON_NOTIFY(UDN_DELTAPOS, SPIN_ID, &CGeneralSettingsDlg::OnDeltaposSpin)
    ON_EN_KILLFOCUS(IDC_MONITOR_SPAN_EDIT, &CGeneralSettingsDlg::OnEnKillfocusMonitorSpanEdit)
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

	((CButton*)GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO))->SetCheck(!m_data.portable_mode);
	((CButton*)GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO))->SetCheck(m_data.portable_mode);
    GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO)->EnableWindow(theApp.m_module_dir_writable);

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
	m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_APPDATA_RADIO), theApp.m_appdata_dir.c_str());
	m_toolTip.AddTool(GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO), theApp.m_module_dir.c_str());

	((CButton*)GetDlgItem(IDC_USE_CPU_TIME_RADIO))->SetCheck(m_data.m_get_cpu_usage_by_cpu_times);
	((CButton*)GetDlgItem(IDC_USE_PDH_RADIO))->SetCheck(!m_data.m_get_cpu_usage_by_cpu_times);

    m_monitor_span_edit.SetRange(MONITOR_TIME_SPAN_MIN, MONITOR_TIME_SPAN_MAX);
    m_monitor_span_edit.SetValue(m_data.monitor_time_span);

    m_monitor_time_span_ori = m_data.monitor_time_span;

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
		MessageBox(CCommon::LoadText(IDS_LANGUAGE_CHANGE_INFO), NULL, MB_ICONINFORMATION | MB_OK);
	}
	m_show_all_interface_modified = (m_data.show_all_interface != theApp.m_general_data.show_all_interface);

	//获取数据文件保存位置的设置
	m_data.portable_mode = (((CButton*)GetDlgItem(IDC_SAVE_TO_PROGRAM_DIR_RADIO))->GetCheck() != 0);
	if (m_data.portable_mode != theApp.m_general_data.portable_mode)
	{
		MessageBox(CCommon::LoadText(IDS_CFG_DIR_CHANGED_INFO), NULL, MB_ICONINFORMATION | MB_OK);
	}

    m_data.monitor_time_span = m_monitor_span_edit.GetValue();

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


void CGeneralSettingsDlg::OnBnClickedUseCpuTimeRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.m_get_cpu_usage_by_cpu_times = true;
}


void CGeneralSettingsDlg::OnBnClickedUsePdhRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.m_get_cpu_usage_by_cpu_times = false;
}

void CGeneralSettingsDlg::OnDeltaposSpin(NMHDR *pNMHDR, LRESULT *pResult)
{
    //这里响应微调按钮（spin button）点击上下按钮时的事件，
    //CSpinButtonCtrl的对象是作为CSpinEdit的成员变量的，而此消息会向CSpinButtonCtrl的父窗口发送，但是CSpinEdit不是它的父窗口，
    //因此此消息无法在CSpinEdit中响应，只能在这里响应。
    //所有CSpinEdit类中的Spin按钮点击时的响应都在这里，因为这些Spin按钮的ID都是“SPIN_ID”。
    //通过GetBuddy的返回值判断微调按钮是属于哪个EditBox的。

    CSpinButtonCtrl* pSpin = (CSpinButtonCtrl*)CWnd::FromHandle(pNMHDR->hwndFrom);
    if (pSpin == nullptr)
        return;
    CWnd* pEdit = pSpin->GetBuddy();
    if(pEdit == &m_monitor_span_edit)       //当用户点击了“监控时间间隔”的微调按钮时
    {
        LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
        if (pNMUpDown->iDelta == -1)
        {
            // 用户按下了spin控件的向下箭头
            int value = m_monitor_span_edit.GetValue();
            value -= MONITOR_SPAN_STEP;
            value /= MONITOR_SPAN_STEP;
            value *= MONITOR_SPAN_STEP;
            m_monitor_span_edit.SetValue(value);
        }
        else if (pNMUpDown->iDelta == 1)
        {
            // 用户按下了spin控件的向上箭头
            int value = m_monitor_span_edit.GetValue();
            value += MONITOR_SPAN_STEP;
            value /= MONITOR_SPAN_STEP;
            value *= MONITOR_SPAN_STEP;
            m_monitor_span_edit.SetValue(value);
        }
        pNMUpDown->iDelta = 0;
    }
    *pResult = 0;
}



void CGeneralSettingsDlg::OnEnKillfocusMonitorSpanEdit()
{
    // TODO: 在此添加控件通知处理程序代码

    //这里限制监控时间间隔只能输入100的倍数
    CString str;
    GetDlgItemText(IDC_MONITOR_SPAN_EDIT, str);
    str.Replace(_T(","), _T(""));
    int value = _ttoi(str.GetString());
    if (value < MONITOR_TIME_SPAN_MIN || value > MONITOR_TIME_SPAN_MAX)
    {
        value = 1000;
    }
    else
    {
        value /= MONITOR_SPAN_STEP;
        value *= MONITOR_SPAN_STEP;
    }
    m_monitor_span_edit.SetValue(value);
}
