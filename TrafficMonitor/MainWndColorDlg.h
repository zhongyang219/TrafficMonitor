#pragma once
#include "ColorStatic.h"
#include "afxwin.h"
#include "ColorSettingListCtrl.h"

// CMainWndColorDlg 对话框

class CMainWndColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainWndColorDlg)

public:
	CMainWndColorDlg(COLORREF colors[MAIN_WND_COLOR_NUM], CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainWndColorDlg();

	const COLORREF* GetColors() const { return m_colors; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_COLOR_DIALOG };
#endif
protected:
	COLORREF m_colors[MAIN_WND_COLOR_NUM];
    CColorSettingListCtrl m_list_ctrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
};
