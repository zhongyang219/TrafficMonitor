// HelpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "MessageDlg.h"
#include "DrawCommon.h"


// CMessageDlg 对话框

#define MESSAGE_DLG_ICON_SIZE (theApp.DPI(32))

IMPLEMENT_DYNAMIC(CMessageDlg, CBaseDialog)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_MESSAGE_DIALOG, pParent)
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

//void CMessageDlg::SetLinkInfo(LPCTSTR text, LPCTSTR url)
//{
//	m_link_text = text;
//	m_link_url = url;
//}

void CMessageDlg::SetMessageIcon(HICON hIcon)
{
    m_icon = hIcon;
}

void CMessageDlg::SetInfoStaticSize(int cx)
{
    if (m_icon != NULL && m_info_static.GetSafeHwnd() != NULL)
    {
        CRect rc_info{ m_rc_info };
        rc_info.left = m_rc_info.left + MESSAGE_DLG_ICON_SIZE + theApp.DPI(8);
        if (cx > 0)
            rc_info.right = cx;
        m_info_static.MoveWindow(rc_info);
    }
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HELP_EDIT, m_message_edit);
	DDX_Control(pDX, IDC_INFO_STATIC, m_info_static);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CBaseDialog)
	ON_WM_GETMINMAXINFO()
	//ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CMessageDlg::OnNMClickSyslink1)
    ON_WM_PAINT()
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CMessageDlg 消息处理程序


BOOL CMessageDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

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

	//CWnd* pLinkCtrl = GetDlgItem(IDC_SYSLINK1);
	//if (pLinkCtrl != nullptr)
	//{
	//	pLinkCtrl->ShowWindow(m_show_link_ctrl);
	//	pLinkCtrl->SetWindowText(_T("<a>") + m_link_text + _T("</a>"));
	//}

    //设置图标的位置
    if (m_icon != NULL)
    {
        CRect rc_edit;
        m_message_edit.GetWindowRect(rc_edit);
        ScreenToClient(rc_edit);
        m_icon_pos.x = rc_edit.left;
        m_icon_pos.y = (rc_edit.top - MESSAGE_DLG_ICON_SIZE) / 2;

        m_info_static.GetWindowRect(m_rc_info);
        ScreenToClient(m_rc_info);
        SetInfoStaticSize(0);
    }

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CMessageDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CBaseDialog::OnGetMinMaxInfo(lpMMI);
}


//void CMessageDlg::OnNMClickSyslink1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	if(!m_link_url.IsEmpty())
//		ShellExecute(NULL, _T("open"), m_link_url, NULL, NULL, SW_SHOW);	//打开超链接
//
//	*pResult = 0;
//}


void CMessageDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CBaseDialog::OnPaint()

    CDrawCommon draw;
    draw.Create(&dc, this);
    draw.DrawIcon(m_icon, m_icon_pos, CSize(MESSAGE_DLG_ICON_SIZE, MESSAGE_DLG_ICON_SIZE));
}


void CMessageDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    SetInfoStaticSize(cx);
}
