#pragma once
#include "afxwin.h"
#include "BaseDialog.h"

// CMessageDlg 对话框

class CMessageDlg : public CBaseDialog
{
	DECLARE_DYNAMIC(CMessageDlg)

public:
	CMessageDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMessageDlg();

	void SetWindowTitle(LPCTSTR str);	//设置窗口标题
	void SetInfoText(LPCTSTR str);		//设置消息标题
	void SetMessageText(LPCTSTR str);	//设置消息文本
	//void ShowLinkStatic(bool show = true) { m_show_link_ctrl = show; }
	//void SetLinkInfo(LPCTSTR text, LPCTSTR url);
    void SetMessageIcon(HICON hIcon);

	//消息对话框的系统标准图标
	enum StandardIcon
	{
		SI_INFORMATION,
		SI_WARNING,
		SI_ERROR
	};

	//为消息对话框框设置系统标准图标
	void SetStandarnMessageIcon(StandardIcon standard_icon);

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

	//CString m_link_text;
	//CString m_link_url;

    HICON m_icon{};
    CPoint m_icon_pos{};        //图标的位置

	//bool m_show_link_ctrl{ false };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	//afx_msg void OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
