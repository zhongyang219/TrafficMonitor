#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "SpinEdit.h"
#include "TabDlg.h"
#include "TaskbarColorDlg.h"

// CTaskBarSettingsDlg 对话框

#define TASKBAR_DEFAULT_STYLE_NUM 3

class CTaskBarSettingsDlg : public CTabDlg
{
	DECLARE_DYNAMIC(CTaskBarSettingsDlg)

public:
    struct TaskBarStyleData
    {
        COLORREF text_colors[TASKBAR_COLOR_NUM]{};
        COLORREF back_color{};
        COLORREF transparent_color{};
        COLORREF status_bar_color{};
        bool specify_each_item_color{};
    };

public:
	CTaskBarSettingsDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskBarSettingsDlg();

	//选项设置数据
	TaskBarSettingData m_data;

    TaskBarStyleData m_default_style[TASKBAR_DEFAULT_STYLE_NUM];    //预设样式

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_SETTINGS_DIALOG };
#endif

protected:
	//控件变量
	CColorStatic m_text_color_static;
	CColorStatic m_back_color_static;
	CColorStatic m_trans_color_static;
	CColorStatic m_status_bar_color_static;
	CToolTipCtrl m_toolTip;
	CComboBox m_unit_combo;
	CButton m_hide_unit_chk;
	CSpinEdit m_font_size_edit;
	CComboBox m_double_click_combo;
	CComboBox m_digit_number_combo;
    CMenu m_default_style_menu;

protected:
	void DrawStaticColor();
	void IniUnitCombo();

    void LoadConfig();
    void SaveConfig() const;
    void ApplyDefaultStyle(int index);      //应用一个预设方案
    void ModifyDefaultStyle(int index);     //将当前颜色设置保存到一个预设方案

    void EnableControl();

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
	afx_msg void OnBnClickedShowStatusBarCheck();
	afx_msg void OnBnClickedSeparateValueUnitCheck();
	afx_msg void OnBnClickedUnitByteRadio();
	afx_msg void OnBnClickedUnitBitRadio();
    afx_msg void OnBnClickedShowToolTipChk();
	//afx_msg void OnBnClickedSetLightMode();
    afx_msg void OnDefaultStyle1();
    afx_msg void OnDefaultStyle2();
    afx_msg void OnDefaultStyle3();
    afx_msg void OnModifyDefaultStyle1();
    afx_msg void OnModifyDefaultStyle2();
    afx_msg void OnModifyDefaultStyle3();
    afx_msg void OnLightModeStyle();
    afx_msg void OnBnClickedDefaultStyleButton();
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedBrowseButton();
};
