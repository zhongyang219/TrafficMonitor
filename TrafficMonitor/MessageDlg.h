#pragma once
#include "afxwin.h"

// CMessageDlg 对话框

class CMessageDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMessageDlg();

	void SetWindowTitle(LPCTSTR str);
	void SetInfoText(LPCTSTR str);
	void SetMessageText(LPCTSTR str);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MESSAGE_DIALOG };
#endif

protected:
	CEdit m_message_edit;
	CSize m_min_size;		//窗口的最小大小
	CStatic m_info_static;

	CString m_title;
	CString m_info;
	CString m_message;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
