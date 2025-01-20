#pragma once
#include "TabDlg.h"
#include "SpinEdit.h"
#include "ComboBox2.h"

// CGeneralSettingsDlg dialog

class CGeneralSettingsDlg : public CTabDlg
{
    DECLARE_DYNAMIC(CGeneralSettingsDlg)

public:
    CGeneralSettingsDlg(CWnd* pParent = NULL);   // standard constructor
    virtual ~CGeneralSettingsDlg();

    static void CheckTaskbarDisplayItem();

    //选项设置数据
    GeneralSettingData m_data;

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_GENERAL_SETTINGS_DIALOG };
#endif

public:
    bool IsAutoRunModified() const { return m_auto_run_modified; }
    bool IsShowAllInterfaceModified() const { return m_show_all_interface_modified; }
    bool IsMonitorTimeSpanModified() const;
    //bool IsTaskbarItemModified() const { return m_taskbar_item_modified; }

protected:
    bool m_auto_run_modified{ false };      //如果更改了开机自动运行的设置，则会置为true
    bool m_show_all_interface_modified{ false };
    int m_monitor_time_span_ori{};
    int m_update_source_ori{};
    //bool m_taskbar_item_modified{ false };
    wstring m_auto_run_path;

    //控件变量
    CSpinEdit m_traffic_tip_edit;
    CComboBox2 m_traffic_tip_combo;
    CSpinEdit m_memory_tip_edit;
    CComboBox2 m_language_combo;
    CToolTipCtrl m_toolTip;
    CSpinEdit m_monitor_span_edit;
    CSpinEdit m_cpu_temp_tip_edit;
    CSpinEdit m_gpu_temp_tip_edit;
    CSpinEdit m_hdd_temp_tip_edit;
    CSpinEdit m_mbd_temp_tip_edit;
    CComboBox2 m_hard_disk_combo;
    CComboBox2 m_select_cpu_combo;
    CButton m_plugin_manager_btn;
    CButton m_select_connection_btn;

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    //设置控件的启用和禁用
    void SetControlEnable();

    virtual void SetControlMouseWheelEnable(bool enable) override;

    //显示开启硬件监控时的提示，如果用户选择了“是”则返回true，否则返回false
    //“以后不再显示该对话框”的标记保存在注册表“\HKEY_CURRENT_USER\SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\DontShowMeThisDialogAgain”
    bool ShowHardwareMonitorWarning();

    //添加或更新开机自动运行的鼠标提示
    void AddOrUpdateAutoRunTooltip(bool add);

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedCheckNowButton();
    afx_msg void OnBnClickedCheckUpdateCheck();
    afx_msg void OnBnClickedAutoRunCheck();
    afx_msg void OnBnClickedAllowSkinFontCheck();
    afx_msg void OnBnClickedAllowSkinDispStrCheck();
    virtual void OnOK();
    afx_msg void OnBnClickedTodayTrafficTipCheck();
    afx_msg void OnBnClickedMemoryUsageTipCheck();
    afx_msg void OnBnClickedOpenConfigPathButton();
    afx_msg void OnBnClickedShowAllConnectionCheck();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedUseCpuTimeRadio();
    afx_msg void OnBnClickedUsePdhRadio();
    afx_msg void OnBnClickedUseHardwareMonitorRadio();
    afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnKillfocusMonitorSpanEdit();
    afx_msg void OnBnClickedCpuTempTipCheck();
    afx_msg void OnBnClickedGpuTempTipCheck();
    afx_msg void OnBnClickedHddTempTipCheck();
    afx_msg void OnBnClickedMbdTempTipCheck();
    afx_msg void OnBnClickedGithubRadio();
    afx_msg void OnBnClickedGiteeRadio();
    virtual void OnCancel();
    afx_msg void OnBnClickedRestoreDefaultTimeSpanButton();
    afx_msg void OnCbnSelchangeSelectHardDiskCombo();
    afx_msg void OnBnClickedCpuCheck();
    afx_msg void OnBnClickedGpuCheck();
    afx_msg void OnBnClickedHddCheck();
    afx_msg void OnBnClickedMbdCheck();
    afx_msg void OnCbnSelchangeSelectCpuCombo();
    afx_msg void OnBnClickedPluginManageButton();
    afx_msg void OnBnClickedShowNotifyIconCheck();
    afx_msg void OnBnClickedSelectConnectionsButton();
    afx_msg void OnBnClickedResetAutoRunButton();
};
