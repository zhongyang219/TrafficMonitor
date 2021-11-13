#pragma once
#include "BaseDialog.h"
#include "CommonData.h"
#include "ListCtrlEx.h"

// CDisplayTextSettingDlg 对话框

class CDisplayTextSettingDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CDisplayTextSettingDlg)

public:
	CDisplayTextSettingDlg(DispStrings& display_texts, bool main_window_text = false, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDisplayTextSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAY_TEXT_SETTINGS_DIALOG };
#endif

private:
    DispStrings& m_display_texts;

    CListCtrlEx m_list_ctrl;
    bool m_main_window_text{ false };       //如果为true，则为主窗口文本设置，否则为任务栏窗口设置

protected:
    virtual CString GetDialogName() const override;
    CommonDisplayItem GetDisplayItem(int row);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedRestoreDefaultButton();
};
