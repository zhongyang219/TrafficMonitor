// CMFCColorDialogEx.cpp: 实现文件
//

#include "stdafx.h"
#include "CMFCColorDialogEx.h"
#include "Common.h"

// CMFCColorDialogEx

//IMPLEMENT_DYNAMIC(CMFCColorDialogEx, CMFCColorDialog)

CMFCColorDialogEx::CMFCColorDialogEx(COLORREF clrInit, DWORD dwFlags, CWnd* pParentWnd, HPALETTE hPal)
	:CMFCColorDialog(clrInit, dwFlags, pParentWnd, hPal)
{

}

CMFCColorDialogEx::~CMFCColorDialogEx()
{
}


BEGIN_MESSAGE_MAP(CMFCColorDialogEx, CMFCColorDialog)
END_MESSAGE_MAP()



// CMFCColorDialogEx 消息处理程序

BOOL CMFCColorDialogEx::OnInitDialog()
{
	CMFCColorDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//设置控件字体
	CWnd* pParent = GetParent();
	if (pParent != nullptr)
	{
		CCommon::SetDialogFont(this, pParent->GetFont());

		CWnd* pPropSheet = m_pColourSheetOne->GetParent();
		if (pPropSheet != nullptr)
			CCommon::SetDialogFont(pPropSheet, pParent->GetFont());

		//CCommon::SetDialogFont(m_pColourSheetOne, pParent->GetFont());
		//CCommon::SetDialogFont(m_pColourSheetTwo, pParent->GetFont());
	}


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

