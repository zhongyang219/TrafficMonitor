// GeneralSettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "GeneralSettingsDlg.h"
#include "PluginManagerDlg.h"
#include "SelectConnectionsDlg.h"


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

void CGeneralSettingsDlg::CheckTaskbarDisplayItem()
{
    //如果选项设置中关闭了某个硬件监控，则不显示对应的温度监控相关项目
    int taskbar_displat_item_ori = theApp.m_taskbar_data.m_tbar_display_item;
    if (!theApp.m_general_data.IsHardwareEnable(HI_CPU))
    {
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_CPU_TEMP;
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_GPU))
    {
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_GPU_USAGE;
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_GPU_TEMP;
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_HDD))
    {
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_HDD_TEMP;
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_HDD_USAGE;
    }
    if (!theApp.m_general_data.IsHardwareEnable(HI_MBD))
        theApp.m_taskbar_data.m_tbar_display_item &= ~TDI_MAIN_BOARD_TEMP;
}

void CGeneralSettingsDlg::SetControlMouseWheelEnable(bool enable)
{
    m_traffic_tip_combo.SetMouseWheelEnable(enable);
    m_language_combo.SetMouseWheelEnable(enable);
    m_traffic_tip_edit.SetMouseWheelEnable(enable);
    m_memory_tip_edit.SetMouseWheelEnable(enable);
    m_monitor_span_edit.SetMouseWheelEnable(enable);
    m_cpu_temp_tip_edit.SetMouseWheelEnable(enable);
    m_gpu_temp_tip_edit.SetMouseWheelEnable(enable);
    m_hdd_temp_tip_edit.SetMouseWheelEnable(enable);
    m_mbd_temp_tip_edit.SetMouseWheelEnable(enable);
    m_hard_disk_combo.SetMouseWheelEnable(enable);
    m_select_cpu_combo.SetMouseWheelEnable(enable);
}

void CGeneralSettingsDlg::OnSettingsApplied()
{
    //当设置被应用时，重置xxxx_ori的值
    m_monitor_time_span_ori = m_data.monitor_time_span;
    m_update_source_ori = m_data.update_source;
}

bool CGeneralSettingsDlg::ShowHardwareMonitorWarning()
{
    //如果已经有硬件监控项目被勾选了，则不再弹出提示
    if (m_data.hardware_monitor_item != 0)
        return true;

    if (SHMessageBoxCheck(m_hWnd, CCommon::LoadText(IDS_HARDWARE_MONITOR_WARNING), APP_NAME, MB_OKCANCEL | MB_ICONWARNING, IDOK, _T("{B8A281A7-76DF-4F0F-BF6A-1A394EF8BAD5}")) == IDOK)
        return true;

    return false;
}

void CGeneralSettingsDlg::AddOrUpdateAutoRunTooltip(bool add)
{
    CString str_tool_tip;
#ifdef WITHOUT_TEMPERATURE
    str_tool_tip = CCommon::LoadText(IDS_AUTO_RUN_METHOD_REGESTRY);
#else
    str_tool_tip = CCommon::LoadText(IDS_AUTO_RUN_METHOD_TASK_SCHEDULE);
#endif
    if (!m_auto_run_path.empty())
    {
        str_tool_tip += _T("\r\n");
        str_tool_tip += CCommon::LoadText(IDS_PATH, _T(": "));
        str_tool_tip += m_auto_run_path.c_str();
    }
    if (add)
        m_toolTip.AddTool(GetDlgItem(IDC_AUTO_RUN_CHECK), str_tool_tip);
    else
        m_toolTip.UpdateTipText(str_tool_tip, GetDlgItem(IDC_AUTO_RUN_CHECK));

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
    DDX_Control(pDX, IDC_CPU_TEMP_TIP_EDIT, m_cpu_temp_tip_edit);
    DDX_Control(pDX, IDC_GPU_TEMP_TIP_EDIT, m_gpu_temp_tip_edit);
    DDX_Control(pDX, IDC_HDD_TIP_EDIT, m_hdd_temp_tip_edit);
    DDX_Control(pDX, IDC_MBD_TEMP_TIP_EDIT, m_mbd_temp_tip_edit);
    DDX_Control(pDX, IDC_SELECT_HARD_DISK_COMBO, m_hard_disk_combo);
    DDX_Control(pDX, IDC_SELECT_CPU_COMBO, m_select_cpu_combo);
    DDX_Control(pDX, IDC_PLUGIN_MANAGE_BUTTON, m_plugin_manager_btn);
    DDX_Control(pDX, IDC_SELECT_CONNECTIONS_BUTTON, m_select_connection_btn);
}

void CGeneralSettingsDlg::SetControlEnable()
{
    m_traffic_tip_edit.EnableWindow(m_data.traffic_tip_enable);
    m_traffic_tip_combo.EnableWindow(m_data.traffic_tip_enable);
    m_memory_tip_edit.EnableWindow(m_data.memory_usage_tip.enable);
    m_cpu_temp_tip_edit.EnableWindow(m_data.cpu_temp_tip.enable);
    m_gpu_temp_tip_edit.EnableWindow(m_data.gpu_temp_tip.enable);
    m_hdd_temp_tip_edit.EnableWindow(m_data.hdd_temp_tip.enable);
    m_mbd_temp_tip_edit.EnableWindow(m_data.mainboard_temp_tip.enable);

    m_hard_disk_combo.EnableWindow(m_data.IsHardwareEnable(HI_HDD));
    m_select_cpu_combo.EnableWindow(m_data.IsHardwareEnable(HI_CPU));
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
    ON_BN_CLICKED(IDC_CPU_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedCpuTempTipCheck)
    ON_BN_CLICKED(IDC_GPU_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedGpuTempTipCheck)
    ON_BN_CLICKED(IDC_HDD_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedHddTempTipCheck)
    ON_BN_CLICKED(IDC_MBD_TEMP_TIP_CHECK, &CGeneralSettingsDlg::OnBnClickedMbdTempTipCheck)
    ON_BN_CLICKED(IDC_GITHUB_RADIO, &CGeneralSettingsDlg::OnBnClickedGithubRadio)
    ON_BN_CLICKED(IDC_GITEE_RADIO, &CGeneralSettingsDlg::OnBnClickedGiteeRadio)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_TIME_SPAN_BUTTON, &CGeneralSettingsDlg::OnBnClickedRestoreDefaultTimeSpanButton)
    ON_CBN_SELCHANGE(IDC_SELECT_HARD_DISK_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeSelectHardDiskCombo)
    ON_BN_CLICKED(IDC_CPU_CHECK, &CGeneralSettingsDlg::OnBnClickedCpuCheck)
    ON_BN_CLICKED(IDC_GPU_CHECK, &CGeneralSettingsDlg::OnBnClickedGpuCheck)
    ON_BN_CLICKED(IDC_HDD_CHECK, &CGeneralSettingsDlg::OnBnClickedHddCheck)
    ON_BN_CLICKED(IDC_MBD_CHECK, &CGeneralSettingsDlg::OnBnClickedMbdCheck)
    ON_CBN_SELCHANGE(IDC_SELECT_CPU_COMBO, &CGeneralSettingsDlg::OnCbnSelchangeSelectCpuCombo)
    ON_BN_CLICKED(IDC_PLUGIN_MANAGE_BUTTON, &CGeneralSettingsDlg::OnBnClickedPluginManageButton)
    ON_BN_CLICKED(IDC_SHOW_NOTIFY_ICON_CHECK, &CGeneralSettingsDlg::OnBnClickedShowNotifyIconCheck)
    ON_BN_CLICKED(IDC_SELECT_CONNECTIONS_BUTTON, &CGeneralSettingsDlg::OnBnClickedSelectConnectionsButton)
    ON_BN_CLICKED(IDC_RESET_AUTO_RUN_BUTTON, &CGeneralSettingsDlg::OnBnClickedResetAutoRunButton)
    ON_BN_CLICKED(IDC_USE_HARDWARE_MONITOR_RADIO, &CGeneralSettingsDlg::OnBnClickedUseHardwareMonitorRadio)
    ON_EN_CHANGE(IDC_MONITOR_SPAN_EDIT, &CGeneralSettingsDlg::OnEnChangeMonitorSpanEdit)
END_MESSAGE_MAP()


// CGeneralSettingsDlg 消息处理程序


BOOL CGeneralSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    ((CButton*)GetDlgItem(IDC_CHECK_UPDATE_CHECK))->SetCheck(m_data.check_update_when_start);
    ((CButton*)GetDlgItem(IDC_ALLOW_SKIN_FONT_CHECK))->SetCheck(m_data.allow_skin_cover_font);
    ((CButton*)GetDlgItem(IDC_ALLOW_SKIN_DISP_STR_CHECK))->SetCheck(m_data.allow_skin_cover_text);
    if (theApp.IsForceShowNotifyIcon())
    {
        m_data.show_notify_icon = true;
        EnableDlgCtrl(IDC_SHOW_NOTIFY_ICON_CHECK, FALSE);
    }
    CheckDlgButton(IDC_SHOW_NOTIFY_ICON_CHECK, m_data.show_notify_icon);

    if (m_data.update_source == 0)
        CheckDlgButton(IDC_GITHUB_RADIO, TRUE);
    else
        CheckDlgButton(IDC_GITEE_RADIO, TRUE);

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
        m_data.auto_run = theApp.GetAutoRun(&m_auto_run_path);
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
    CheckDlgButton(IDC_MEMORY_USAGE_TIP_CHECK, m_data.memory_usage_tip.enable);
    m_memory_tip_edit.SetRange(1, 100);
    m_memory_tip_edit.SetValue(m_data.memory_usage_tip.tip_value);

    CheckDlgButton(IDC_CPU_TEMP_TIP_CHECK, m_data.cpu_temp_tip.enable);
    m_cpu_temp_tip_edit.SetRange(1, 120);
    m_cpu_temp_tip_edit.SetValue(m_data.cpu_temp_tip.tip_value);

    CheckDlgButton(IDC_GPU_TEMP_TIP_CHECK, m_data.gpu_temp_tip.enable);
    m_gpu_temp_tip_edit.SetRange(1, 120);
    m_gpu_temp_tip_edit.SetValue(m_data.gpu_temp_tip.tip_value);

    CheckDlgButton(IDC_HDD_TEMP_TIP_CHECK, m_data.hdd_temp_tip.enable);
    m_hdd_temp_tip_edit.SetRange(1, 120);
    m_hdd_temp_tip_edit.SetValue(m_data.hdd_temp_tip.tip_value);

    CheckDlgButton(IDC_MBD_TEMP_TIP_CHECK, m_data.mainboard_temp_tip.enable);
    m_mbd_temp_tip_edit.SetRange(1, 120);
    m_mbd_temp_tip_edit.SetValue(m_data.mainboard_temp_tip.tip_value);

    SetControlEnable();

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
    AddOrUpdateAutoRunTooltip(true);

    if (m_data.cpu_usage_acquire_method == GeneralSettingData::CA_CPU_TIME)
    {
        CheckDlgButton(IDC_USE_CPU_TIME_RADIO, TRUE);
    }
    else if (m_data.cpu_usage_acquire_method == GeneralSettingData::CA_PDH)
    {
        CheckDlgButton(IDC_USE_PDH_RADIO, TRUE);
    }
    else if (m_data.cpu_usage_acquire_method == GeneralSettingData::CA_HARDWARE_MONITOR)
    {
        if (m_data.IsHardwareEnable(HI_CPU))
            CheckDlgButton(IDC_USE_HARDWARE_MONITOR_RADIO, TRUE);
        else
            CheckDlgButton(IDC_USE_CPU_TIME_RADIO, TRUE);
    }

#ifndef WITHOUT_TEMPERATURE
    EnableDlgCtrl(IDC_USE_HARDWARE_MONITOR_RADIO, m_data.IsHardwareEnable(HI_CPU));
#else
    EnableDlgCtrl(IDC_USE_HARDWARE_MONITOR_RADIO, false);
#endif

    m_monitor_span_edit.SetRange(MONITOR_TIME_SPAN_MIN, MONITOR_TIME_SPAN_MAX);
    m_monitor_span_edit.SetValue(m_data.monitor_time_span);

    m_monitor_time_span_ori = m_data.monitor_time_span;
    m_update_source_ori = m_data.update_source;

#ifndef WITHOUT_TEMPERATURE
    //初始化硬件监控Check box
    CheckDlgButton(IDC_CPU_CHECK, m_data.IsHardwareEnable(HI_CPU));
    CheckDlgButton(IDC_GPU_CHECK, m_data.IsHardwareEnable(HI_GPU));
    CheckDlgButton(IDC_HDD_CHECK, m_data.IsHardwareEnable(HI_HDD));
    CheckDlgButton(IDC_MBD_CHECK, m_data.IsHardwareEnable(HI_MBD));

    if (theApp.m_pMonitor != nullptr)
    {
        CSingleLock sync(&theApp.m_minitor_lib_critical, TRUE);
        //初始化选择硬盘下拉列表
        for (const auto& hdd_item : theApp.m_pMonitor->AllHDDTemperature())
            m_hard_disk_combo.AddString(hdd_item.first.c_str());
        int cur_index = m_hard_disk_combo.FindString(-1, m_data.hard_disk_name.c_str());
        m_hard_disk_combo.SetCurSel(cur_index);
        //初始化选择CPU下拉列表
        m_select_cpu_combo.AddString(CCommon::LoadText(IDS_AVREAGE_TEMPERATURE));
        for (const auto& cpu_item : theApp.m_pMonitor->AllCpuTemperature())
            m_select_cpu_combo.AddString(cpu_item.first.c_str());
        cur_index = m_select_cpu_combo.FindString(-1, m_data.cpu_core_name.c_str());
        if (cur_index < 0)
            cur_index = 0;
        m_select_cpu_combo.SetCurSel(cur_index);
    }
#endif

    //不含温度监控的版本，禁用温度相关的控件
#ifdef WITHOUT_TEMPERATURE
    EnableDlgCtrl(IDC_CPU_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_CPU_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_GPU_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_GPU_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_HDD_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_HDD_TIP_EDIT, false);
    EnableDlgCtrl(IDC_MBD_TEMP_TIP_CHECK, false);
    EnableDlgCtrl(IDC_MBD_TEMP_TIP_EDIT, false);
    EnableDlgCtrl(IDC_CPU_CHECK, false);
    EnableDlgCtrl(IDC_GPU_CHECK, false);
    EnableDlgCtrl(IDC_HDD_CHECK, false);
    EnableDlgCtrl(IDC_MBD_CHECK, false);
    EnableDlgCtrl(IDC_SELECT_HARD_DISK_COMBO, false);
    EnableDlgCtrl(IDC_SELECT_CPU_COMBO, false);
    EnableDlgCtrl(IDC_CPU_TEMP_STATIC, false);
    EnableDlgCtrl(IDC_GPU_TEMP_STATIC, false);
    EnableDlgCtrl(IDC_HDD_STATIC, false);
    EnableDlgCtrl(IDC_MBD_TEMP_STATIC, false);
    EnableDlgCtrl(IDC_SELECT_HDD_STATIC, false);
    EnableDlgCtrl(IDC_SELECT_CPU_STATIC, false);
    EnableDlgCtrl(IDC_HARDWARE_MONITOR_STATIC, false);
#endif

    m_plugin_manager_btn.SetIcon(theApp.GetMenuIcon(IDI_PLUGINS));
    m_select_connection_btn.SetIcon(theApp.GetMenuIcon(IDI_CONNECTION));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CGeneralSettingsDlg::OnBnClickedCheckNowButton()
{
    // TODO: 在此添加控件通知处理程序代码
    theApp.CheckUpdateInThread(true);
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

    auto checkTipValue = [](int& value)
    {
        if (value < 1) value = 1;
        if (value > 100) value = 100;
    };
    auto checkTempTipValue = [](int& value)
    {
        if (value < 1) value = 1;
        if (value > 120) value = 120;
    };
    m_data.memory_usage_tip.tip_value = m_memory_tip_edit.GetValue();
    checkTipValue(m_data.memory_usage_tip.tip_value);

    m_data.cpu_temp_tip.tip_value = m_cpu_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.cpu_temp_tip.tip_value);

    m_data.gpu_temp_tip.tip_value = m_gpu_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.gpu_temp_tip.tip_value);

    m_data.hdd_temp_tip.tip_value = m_hdd_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.hdd_temp_tip.tip_value);

    m_data.mainboard_temp_tip.tip_value = m_mbd_temp_tip_edit.GetValue();
    checkTempTipValue(m_data.mainboard_temp_tip.tip_value);

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

    //m_taskbar_item_modified = (theApp.m_taskbar_data.m_tbar_display_item != taskbar_displat_item_ori);

    CTabDlg::OnOK();
}


void CGeneralSettingsDlg::OnBnClickedTodayTrafficTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.traffic_tip_enable = (((CButton*)GetDlgItem(IDC_TODAY_TRAFFIC_TIP_CHECK))->GetCheck() != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedMemoryUsageTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.memory_usage_tip.enable = (((CButton*)GetDlgItem(IDC_MEMORY_USAGE_TIP_CHECK))->GetCheck() != 0);
    SetControlEnable();
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
    m_data.cpu_usage_acquire_method = GeneralSettingData::CA_CPU_TIME;
}


void CGeneralSettingsDlg::OnBnClickedUsePdhRadio()
{
    m_data.cpu_usage_acquire_method = GeneralSettingData::CA_PDH;
}

void CGeneralSettingsDlg::OnBnClickedUseHardwareMonitorRadio()
{
    m_data.cpu_usage_acquire_method = GeneralSettingData::CA_HARDWARE_MONITOR;
}

void CGeneralSettingsDlg::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
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
    if (pEdit == &m_monitor_span_edit)       //当用户点击了“监控时间间隔”的微调按钮时
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


void CGeneralSettingsDlg::OnBnClickedCpuTempTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.cpu_temp_tip.enable = (IsDlgButtonChecked(IDC_CPU_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedGpuTempTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.gpu_temp_tip.enable = (IsDlgButtonChecked(IDC_GPU_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedHddTempTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hdd_temp_tip.enable = (IsDlgButtonChecked(IDC_HDD_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedMbdTempTipCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.mainboard_temp_tip.enable = (IsDlgButtonChecked(IDC_MBD_TEMP_TIP_CHECK) != 0);
    SetControlEnable();
}


void CGeneralSettingsDlg::OnBnClickedGithubRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = 0;
    theApp.m_general_data.update_source = 0;        //点击“更新源”的单选按钮时，同时更改theApp中的设置，以确保点击“立即检查”按钮时使用选择的更新源更新
}


void CGeneralSettingsDlg::OnBnClickedGiteeRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.update_source = 1;
    theApp.m_general_data.update_source = 1;
}


void CGeneralSettingsDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    theApp.m_general_data.update_source = m_update_source_ori;      //点击“取消”时恢复开始的“更新源”选项

    CTabDlg::OnCancel();
}


void CGeneralSettingsDlg::OnBnClickedRestoreDefaultTimeSpanButton()
{
    // TODO: 在此添加控件通知处理程序代码
    m_monitor_span_edit.SetValue(1000);
}


void CGeneralSettingsDlg::OnCbnSelchangeSelectHardDiskCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    CString hard_disk_name;
    m_hard_disk_combo.GetWindowText(hard_disk_name);
    m_data.hard_disk_name = hard_disk_name.GetString();
}


void CGeneralSettingsDlg::OnBnClickedCpuCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = IsDlgButtonChecked(IDC_CPU_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_CPU_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_CPU, checked);
    EnableDlgCtrl(IDC_USE_HARDWARE_MONITOR_RADIO, checked);
}


void CGeneralSettingsDlg::OnBnClickedGpuCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = IsDlgButtonChecked(IDC_GPU_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_GPU_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_GPU, checked);
}


void CGeneralSettingsDlg::OnBnClickedHddCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = IsDlgButtonChecked(IDC_HDD_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_HDD_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_HDD, checked);
}


void CGeneralSettingsDlg::OnBnClickedMbdCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = IsDlgButtonChecked(IDC_MBD_CHECK) != 0;
    if (checked && !ShowHardwareMonitorWarning())
    {
        checked = false;
        CheckDlgButton(IDC_MBD_CHECK, FALSE);
    }
    m_data.SetHardwareEnable(HI_MBD, checked);

}


void CGeneralSettingsDlg::OnCbnSelchangeSelectCpuCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    CString cpu_core_name;
    m_select_cpu_combo.GetWindowText(cpu_core_name);
    m_data.cpu_core_name = cpu_core_name.GetString();
}


void CGeneralSettingsDlg::OnBnClickedPluginManageButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CPluginManagerDlg dlg;
    dlg.DoModal();
}


void CGeneralSettingsDlg::OnBnClickedShowNotifyIconCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_notify_icon = (IsDlgButtonChecked(IDC_SHOW_NOTIFY_ICON_CHECK) != 0);
}


void CGeneralSettingsDlg::OnBnClickedSelectConnectionsButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CSelectConnectionsDlg dlg(m_data.connections_hide);
    if (dlg.DoModal() == IDOK)
    {
        m_data.connections_hide = dlg.GetData();
    }
}


void CGeneralSettingsDlg::OnBnClickedResetAutoRunButton()
{
    //先删除开机自动运行
    theApp.SetAutoRunByRegistry(false);
    theApp.SetAutoRunByTaskScheduler(false);
    if (!theApp.SetAutoRun(true))    //重新设置开机自动运行
    {
        MessageBox(CCommon::LoadText(IDS_SET_AUTO_RUN_FAILED_WARNING), NULL, MB_ICONWARNING | MB_OK);
        return;
    }
    //获取开机自动运行的路径
    bool auto_run = theApp.GetAutoRun(&m_auto_run_path);
    //重新勾选“开机自动运行”复选框
    CheckDlgButton(IDC_AUTO_RUN_CHECK, auto_run);
    //更新鼠标提示
    AddOrUpdateAutoRunTooltip(false);
}


void CGeneralSettingsDlg::OnEnChangeMonitorSpanEdit()
{
    m_data.monitor_time_span = m_monitor_span_edit.GetValue();
}
