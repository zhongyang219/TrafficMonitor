#include "stdafx.h"
#include "TabDlg.h"


CTabDlg::CTabDlg(UINT nIDTemplate, CWnd * pParent) : CDialogEx(nIDTemplate, pParent)
{
	m_pParent = pParent;
}


CTabDlg::~CTabDlg()
{
}


BOOL CTabDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	//由于这是tab标签中的子对话框，因此用户按回车或ESC后不应该响应当前对话框的IDOK或IDCANCEL，
	//而应该响应tab标签的父窗口的IDOK或IDCANCEL命令
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		if (m_pParent != nullptr)
			m_pParent->SendMessage(WM_COMMAND, IDCANCEL);
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		if (m_pParent != nullptr)
			m_pParent->SendMessage(WM_COMMAND, IDOK);
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


BOOL CTabDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//将窗口背景设置成白色
	SetBackgroundColor(RGB(255, 255, 255));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
