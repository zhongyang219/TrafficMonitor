#pragma once
#include "TabDlg.h"
#include "SpinEdit.h"

// CGeneralSettingsDlg dialog

class CGeneralSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CGeneralSettingsDlg)

public:
	CGeneralSettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeneralSettingsDlg();

	//选项设置数据
	GeneralSettingData m_data;

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GENERAL_SETTINGS_DIALOG };
#endif

public:
	bool IsAutoRunModified() const { return m_auto_run_modified; }
	bool IsShowAllInterfaceModified() const { return m_show_all_interface_modified; }

protected:
	bool m_auto_run_modified{ false };		//如果更改了开机自动运行的设置，则会置为true
	bool m_show_all_interface_modified{ false };

	//控件变量
	CSpinEdit m_traffic_tip_edit;
	CComboBox m_traffic_tip_combo;
	CSpinEdit m_memory_tip_edit;
	CComboBox m_language_combo;
	CToolTipCtrl m_toolTip;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//设置控件的启用和禁用
	void SetTrafficTipControlEnable(bool enable);
	void SetMemoryTipControlEnable(bool enable);

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
};
