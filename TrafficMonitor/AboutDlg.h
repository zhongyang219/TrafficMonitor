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
	CPictureStatic m_about_img;		//关于对话框中的图片
	CLinkStatic m_mail;				//“联系作者”超链接
	CLinkStatic m_acknowledgement;	//“鸣谢”超链接
	CLinkStatic m_github;			//“GitHub”超链接
	CLinkStatic m_donate;			//“捐助”超链接
	CToolTipCtrl m_tool_tip;			//鼠标指向时的工具提示
	CLinkStatic m_translaotr_static;

	//布局数据
	//DPI设定为100%时的大小
	const int m_width{ theApp.DPI(305) };
	const int m_height{ theApp.DPI(174) };
	const int m_pic_height{ theApp.DPI(65) };
	//DPI设定大于100%时的大小
	const int m_width2{ theApp.DPI(319) };
	const int m_height2{ theApp.DPI(164) };
	const int m_pic_height2{ theApp.DPI(61) };

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
