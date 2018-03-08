#pragma once
#include "StaticEx.h"
#include "afxwin.h"

// CMainWndSettingsDlg 对话框

class CMainWndSettingsDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CMainWndSettingsDlg)

public:
	CMainWndSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWndSettingsDlg();

	//选项设置数据
	MainWndSettingData m_data;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_WND_SETTINGS_DIALOG };
#endif

protected:
	CStaticEx m_color_static;
	CToolTipCtrl m_toolTip;
	CComboBox m_unit_combo;
	CButton m_hide_unit_chk;

	void DrawStaticColor();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeUploadEdit();
	afx_msg void OnEnChangeDownloadEdit();
	afx_msg void OnEnChangeCpuEdit();
	afx_msg void OnEnChangeMemoryEdit();
	afx_msg void OnBnClickedSetColorButton1();
	afx_msg void OnBnClickedSetDefaultButton();
	afx_msg void OnBnClickedSetFontButton();
	afx_msg void OnBnClickedSwitchUpDownCheck();
	afx_msg void OnBnClickedFullscreenHideCheck();
	afx_msg void OnBnClickedSpeedShortModeCheck2();
	afx_msg void OnCbnSelchangeUnitCombo();
	afx_msg void OnBnClickedHideUnitCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
