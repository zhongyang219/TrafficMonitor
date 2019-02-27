// HelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MessageDlg.h"


// CMessageDlg 对话框

IMPLEMENT_DYNAMIC(CMessageDlg, CDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MESSAGE_DIALOG, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::SetWindowTitle(LPCTSTR str)
{
	m_title = str;
}

void CMessageDlg::SetInfoText(LPCTSTR str)
{
	m_info = str;
}

void CMessageDlg::SetMessageText(LPCTSTR str)
{
	m_message = str;
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_EDIT, m_message_edit);
	DDX_Control(pDX, IDC_INFO_STATIC, m_info_static);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CMessageDlg 消息处理程序


BOOL CMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);		// 设置小图标

	//获取初始时窗口的大小
	CRect rect;
	GetWindowRect(rect);
	m_min_size.cx = rect.Width();
	m_min_size.cy = rect.Height();

	SetWindowText(m_title);
	m_info_static.SetWindowText(m_info);
	m_message_edit.SetWindowText(m_message);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMessageDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialog::OnGetMinMaxInfo(lpMMI);
}
