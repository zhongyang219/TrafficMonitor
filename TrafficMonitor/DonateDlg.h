#pragma once
#include "afxwin.h"


// CDonateDlg 对话框

class CDonateDlg : public CDialog
{
	DECLARE_DYNAMIC(CDonateDlg)

public:
	CDonateDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDonateDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DONATE_DIALOG };
#endif

protected:
	CStatic m_donate_pic;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
