#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "ColorSettingListCtrl.h"
#include "BaseDialog.h"

// CMainWndColorDlg 对话框

class CMainWndColorDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CMainWndColorDlg)

public:
	CMainWndColorDlg(const std::map<CommonDisplayItem, COLORREF>& colors, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWndColorDlg();

    const std::map<CommonDisplayItem, COLORREF>& GetColors() const { return m_colors; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_COLOR_DIALOG };
#endif
protected:
    std::map<CommonDisplayItem, COLORREF> m_colors;
    CColorSettingListCtrl m_list_ctrl;

    virtual CString GetDialogName() const override;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
