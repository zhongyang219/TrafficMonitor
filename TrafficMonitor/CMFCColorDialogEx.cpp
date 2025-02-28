// CMFCColorDialogEx.cpp: 实现文件
//

#include "stdafx.h"
#include "CMFCColorDialogEx.h"
#include "Common.h"
#include "TrafficMonitor.h"

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

	//初始化字体
	CRect rect;
	GetWindowRect(rect);
	FontInfo font_info;
	font_info.name = theApp.m_str_table.GetLanguageInfo().default_font_name.c_str();
	font_info.size = 9;
	UINT dpi_x{}, dpi_y{};
	int dpi = theApp.GetDpi();
	if (theApp.DPIFromRect(rect, &dpi_x, &dpi_y))
		dpi = dpi_x;
	font_info.Create(m_dlg_font, dpi);

	CCommon::SetDialogFont(this, &m_dlg_font);

	CWnd* pPropSheet = m_pColourSheetOne->GetParent();
	if (pPropSheet != nullptr)
		CCommon::SetDialogFont(pPropSheet, &m_dlg_font);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

