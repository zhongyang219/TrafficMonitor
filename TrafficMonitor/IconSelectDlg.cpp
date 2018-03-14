// IconSelectDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "IconSelectDlg.h"
#include "afxdialogex.h"


// CIconSelectDlg 对话框

IMPLEMENT_DYNAMIC(CIconSelectDlg, CDialog)

CIconSelectDlg::CIconSelectDlg(int icon_selected, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ICON_SELECT_DIALOG, pParent), m_icon_selected{ icon_selected }
{

}

CIconSelectDlg::~CIconSelectDlg()
{
}

int CIconSelectDlg::GetIconSelected() const
{
	if (m_icon_selected < 0 || m_icon_selected >= MAX_NOTIFY_ICON)
		return 0;
	return m_icon_selected;
}

void CIconSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICON_PREVIEW, m_preview_pic);
	DDX_Control(pDX, IDC_COMBO1, m_icon_select_combo);
}

void CIconSelectDlg::DrawPreviewIcon()
{
	CDC* pDC = m_preview_pic.GetDC();
	pDC->FillSolidRect(CRect(CPoint(ICON_X, ICON_Y), CSize(theApp.DPI(16), theApp.DPI(16))), RGB(0, 0, 0));
	//pDC->DrawIcon(ICON_X, ICON_Y, m_icons[m_icon_selected]);
	::DrawIconEx(pDC->m_hDC, ICON_X, ICON_Y, theApp.m_notify_icons[GetIconSelected()], theApp.DPI(16), theApp.DPI(16), 0, NULL, DI_NORMAL);
}


BEGIN_MESSAGE_MAP(CIconSelectDlg, CDialog)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CIconSelectDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CIconSelectDlg 消息处理程序


BOOL CIconSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//设置预览图大小
	m_preview_pic.SetWindowPos(nullptr, 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT, SWP_NOZORDER | SWP_NOMOVE);
	m_preview_pic.SetPicture(IDB_NOTIFY_ICON_PREVIEW);

	//初始化下拉列表
	m_icon_select_combo.AddString(_T("默认图标"));
	m_icon_select_combo.AddString(_T("图标1"));
	m_icon_select_combo.AddString(_T("图标2"));
	m_icon_select_combo.AddString(_T("图标3"));
	m_icon_select_combo.SetCurSel(m_icon_selected);

	//DrawPreviewIcon();
	SetTimer(2346, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CIconSelectDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 2346)
	{
		DrawPreviewIcon();
		KillTimer(2346);
	}

	CDialog::OnTimer(nIDEvent);
}


void CIconSelectDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_icon_selected = m_icon_select_combo.GetCurSel();
	DrawPreviewIcon();
}
