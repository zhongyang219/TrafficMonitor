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

void CIconSelectDlg::DrawPreviewIcon(CDC* pDC)
{
	//pDC->FillSolidRect(CRect(CPoint(ICON_X, ICON_Y), CSize(theApp.DPI(16), theApp.DPI(16))), RGB(0, 0, 0));
	//pDC->DrawIcon(ICON_X, ICON_Y, m_icons[m_icon_selected]);
	::DrawIconEx(pDC->m_hDC, ICON_X, ICON_Y, theApp.m_notify_icons[GetIconSelected()], theApp.DPI(16), theApp.DPI(16), 0, NULL, DI_NORMAL);
}


BEGIN_MESSAGE_MAP(CIconSelectDlg, CDialog)
	//ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CIconSelectDlg::OnCbnSelchangeCombo1)
	ON_MESSAGE(WM_CONTROL_REPAINT, &CIconSelectDlg::OnControlRepaint)
END_MESSAGE_MAP()


// CIconSelectDlg 消息处理程序


BOOL CIconSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(CCommon::LoadText(IDS_TITLE_CHANGE_ICON));

	//设置预览图大小
	m_preview_pic.SetWindowPos(nullptr, 0, 0, PREVIEW_WIDTH, PREVIEW_HEIGHT, SWP_NOZORDER | SWP_NOMOVE);
	if (m_icon_selected == 4)
		m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW_LIGHT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	else
		m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));

	//初始化下拉列表
	m_icon_select_combo.AddString(CCommon::LoadText(IDS_DEFAULT_ICON));
	m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 1")));
	m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 2")));
	m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 3")));
	m_icon_select_combo.AddString(CCommon::LoadText(IDS_ICON, _T(" 4")));
	m_icon_select_combo.SetCurSel(m_icon_selected);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CIconSelectDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_icon_selected = m_icon_select_combo.GetCurSel();
	if (m_icon_selected == 4)
		m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW_LIGHT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	else
		m_preview_pic.SetPicture((HBITMAP)LoadImage(AfxGetInstanceHandle(),
			MAKEINTRESOURCE(IDB_NOTIFY_ICON_PREVIEW), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION));
	DrawPreviewIcon(m_preview_pic.GetDC());
	}


afx_msg LRESULT CIconSelectDlg::OnControlRepaint(WPARAM wParam, LPARAM lParam)
{
	CWnd* pControl = (CWnd*)wParam;
	CDC* pDC = (CDC*)lParam;
	if (pControl == &m_preview_pic)
	{
		//当收到m_preview_pic控件的重绘消息时，同时重绘图标
		DrawPreviewIcon(pDC);
	}
	return 0;
}
