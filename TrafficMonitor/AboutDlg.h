#pragma once
#include "DonateDlg.h"
#include "LinkStatic.h"

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// 对话框数据

protected:
	CLinkStatic m_mail;				//“联系作者”超链接
	CLinkStatic m_acknowledgement;	//“鸣谢”超链接
	CLinkStatic m_github;			//“GitHub”超链接
	CLinkStatic m_donate;			//“捐助”超链接
	CLinkStatic m_license;			//“开源协议”超链接
	CToolTipCtrl m_tool_tip;			//鼠标指向时的工具提示
	CLinkStatic m_translaotr_static;

    CRect m_rc_pic;     //图片的位置
    CBitmap m_about_pic;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CString GetDonateList();		//从资源文件加载捐赠人员名单

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//	afx_msg void OnStnClickedStaticDonate();
protected:
	afx_msg LRESULT OnLinkClicked(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
};
