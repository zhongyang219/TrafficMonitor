#pragma once
#include "afxdialogex.h"
#include "SpinEdit.h"


// CWin11TaskbarSettingDlg 对话框

class CWin11TaskbarSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CWin11TaskbarSettingDlg)

public:
	CWin11TaskbarSettingDlg(TaskBarSettingData& data, CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CWin11TaskbarSettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WIN11_TASKBAR_SETTING_DLG };
#endif

private:
    TaskBarSettingData& m_data;
    CSpinEdit m_window_offset_top_edit;
    CSpinEdit m_window_offset_left_edit;
    CSpinEdit m_widgets_width_edit;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    void EnableDlgCtrl(UINT id, bool enable);

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnBnClickedRestoreDefaultButton();
};
