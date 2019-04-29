#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "SpinEdit.h"
#include "TabDlg.h"
#include "TaskbarColorDlg.h"

// CTaskBarSettingsDlg 对话框

class CTaskBarSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CTaskBarSettingsDlg)

public:
	CTaskBarSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskBarSettingsDlg();

	//选项设置数据
	TaskBarSettingData m_data;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_SETTINGS_DIALOG };
#endif

protected:
	//控件变量
	CColorStatic m_text_color_static;
	CColorStatic m_back_color_static;
	CColorStatic m_trans_color_static;
	CToolTipCtrl m_toolTip;
	CComboBox m_unit_combo;
	CButton m_hide_unit_chk;
	CSpinEdit m_font_size_edit;
	CComboBox m_double_click_combo;
	CComboBox m_digit_number_combo;

	void DrawStaticColor();
	void IniUnitCombo();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSetFontButton1();
	afx_msg void OnEnChangeUploadEdit1();
	afx_msg void OnEnChangeDownloadEdit1();
	afx_msg void OnEnChangeCpuEdit1();
	afx_msg void OnEnChangeMemoryEdit1();
	afx_msg void OnBnClickedSetDefaultButton1();
	afx_msg void OnBnClickedSwitchUpDownCheck1();
	afx_msg void OnBnClickedTaskbarWndOnLeftCheck();
	afx_msg void OnBnClickedSpeedShortModeCheck();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnCbnSelchangeUnitCombo();
	afx_msg void OnBnClickedHideUnitCheck();
	virtual void OnOK();
	afx_msg void OnBnClickedValueRightAlignCheck();
	afx_msg void OnBnClickedHidePercentageCheck();
protected:
	afx_msg LRESULT OnStaticClicked(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedSpecifyEachItemColorCheck();
	afx_msg void OnCbnSelchangeDoubleClickCombo();
	afx_msg void OnBnClickedHorizontalArrangeCheck();
	afx_msg void OnBnClickedSeparateValueUnitCheck();
	afx_msg void OnBnClickedUnitByteRadio();
	afx_msg void OnBnClickedUnitBitRadio();
};
