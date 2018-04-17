#pragma once
#include "ColorStatic.h"
#include "afxwin.h"

// CTaskbarColorDlg 对话框

class CTaskbarColorDlg : public CDialog
{
	DECLARE_DYNAMIC(CTaskbarColorDlg)

public:
	CTaskbarColorDlg(COLORREF colors[TASKBAR_COLOR_NUM], CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTaskbarColorDlg();

	const COLORREF* GetColors() const { return m_colors; }
	
	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASKBAR_COLOR_DIALOG };
#endif

protected:
	COLORREF m_colors[TASKBAR_COLOR_NUM];

	//控件变量
	CColorStatic m_statics[TASKBAR_COLOR_NUM];		//颜色控件

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnStaticClicked(WPARAM wParam, LPARAM lParam);
};
