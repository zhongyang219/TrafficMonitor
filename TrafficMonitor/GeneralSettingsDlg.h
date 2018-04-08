#pragma once
#include "TabDlg.h"


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

protected:
	bool m_auto_run_modified{ false };		//如果更改了开机自动运行的设置，则会置为true

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckNowButton();
	afx_msg void OnBnClickedCheckUpdateCheck();
	afx_msg void OnBnClickedAutoRunCheck();
};
