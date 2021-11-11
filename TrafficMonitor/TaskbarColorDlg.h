#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "ColorSettingListCtrl.h"
#include "BaseDialog.h"

// CTaskbarColorDlg 对话框

class CTaskbarColorDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CTaskbarColorDlg)

public:
	CTaskbarColorDlg(const std::map<CommonDisplayItem, TaskbarItemColor>& colors, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskbarColorDlg();

    const std::map<CommonDisplayItem, TaskbarItemColor>& GetColors() const { return m_colors; }

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_COLOR_DIALOG };
#endif

protected:
    std::map<CommonDisplayItem, TaskbarItemColor> m_colors;
    CColorSettingListCtrl m_list_ctrl;

    virtual CString GetDialogName() const override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
