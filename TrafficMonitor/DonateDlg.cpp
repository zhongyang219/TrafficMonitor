// DonateDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "DonateDlg.h"
#include "afxdialogex.h"


// CDonateDlg 对话框

IMPLEMENT_DYNAMIC(CDonateDlg, CDialog)

CDonateDlg::CDonateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DONATE_DIALOG, pParent)
{

}

CDonateDlg::~CDonateDlg()
{
}

void CDonateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DONATE_PIC, m_donate_pic);
}


BEGIN_MESSAGE_MAP(CDonateDlg, CDialog)
END_MESSAGE_MAP()


// CDonateDlg 消息处理程序


BOOL CDonateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CWindowDC dc(this);
	HDC hDC = dc.GetSafeHdc();
	int dpi = GetDeviceCaps(hDC, LOGPIXELSY);
	int side = 304 * dpi / 96;

	m_donate_pic.SetWindowPos(&CWnd::wndTop, 0, 0, side, side, SWP_NOMOVE);	//更改控件的宽和高

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
