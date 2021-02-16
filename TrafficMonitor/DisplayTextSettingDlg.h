#pragma once
#include "BaseDialog.h"
#include "CommonData.h"
#include "ListCtrlEx.h"

// CDisplayTextSettingDlg 对话框

class CDisplayTextSettingDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CDisplayTextSettingDlg)

public:
	CDisplayTextSettingDlg(DispStrings& display_texts, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDisplayTextSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DISPLAY_TEXT_SETTINGS_DIALOG };
#endif

private:
    DispStrings& m_display_texts;

    CListCtrlEx m_list_ctrl;

protected:
    virtual CString GetDialogName() const override;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedRestoreDefaultButton();
};
