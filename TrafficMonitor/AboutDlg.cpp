#include "stdafx.h"
#include "TrafficMonitor.h"
#include "AboutDlg.h"
#include "MessageDlg.h"
#include "DrawCommon.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//ON_STN_CLICKED(IDC_STATIC_DONATE, &CAboutDlg::OnStnClickedStaticDonate)
	ON_MESSAGE(WM_LINK_CLICKED, &CAboutDlg::OnLinkClicked)
	ON_WM_PAINT()
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_MAIL, m_mail);
	DDX_Control(pDX, IDC_STATIC_ACKNOWLEDGEMENT, m_acknowledgement);
	DDX_Control(pDX, IDC_STATIC_GITHUB, m_github);
	DDX_Control(pDX, IDC_STATIC_DONATE, m_donate);
	DDX_Control(pDX, IDC_TRANSLATOR_STATIC, m_translaotr_static);
	DDX_Control(pDX, IDC_STATIC_LICENSE, m_license);
}

CString CAboutDlg::GetDonateList()
{
	return CCommon::GetTextResource(IDR_ACKNOWLEDGEMENT_TEXT, 2);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(CCommon::LoadText(IDS_TITLE_ABOUT));
	m_mail.SetURL(_T("mailto:zhongyang219@hotmail.com"));	//设置超链接
															//m_check_update.SetURL(_T("http://pan.baidu.com/s/1c1LkPQ4"));
	m_github.SetURL(_T("https://github.com/zhongyang219/TrafficMonitor"));
	m_donate.SetLinkIsURL(false);
	m_acknowledgement.SetLinkIsURL(false);
	m_license.SetLinkIsURL(false);

	//设置版本信息
	CString version_info;
	GetDlgItemText(IDC_STATIC_VERSION, version_info);
	version_info.Replace(_T("<version>"), VERSION);

#ifdef COMPILE_FOR_WINXP
	version_info += _T(" (For WinXP)");
#endif // COMPILE_FOR_WINXP

#ifdef _M_X64
	version_info += _T(" (x64)");
#endif

#ifdef _DEBUG
	version_info += _T(" (Debug)");
#endif

	SetDlgItemText(IDC_STATIC_VERSION, version_info);

	//设置最后编译日期
	CString temp_str;
	GetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);
	temp_str.Replace(_T("<compile_date>"), COMPILE_DATE);
	SetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);

	m_tool_tip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_tool_tip.AddTool(&m_mail, CCommon::LoadText(IDS_SEND_EMAIL_TO_ATHOUR, _T("\r\nmailto:zhongyang219@hotmail.com")));
	//m_tool_tip.AddTool(&m_check_update, _T("到百度网盘链接查看是否有更新\r\nhttp://pan.baidu.com/s/1c1LkPQ4"));
	m_tool_tip.AddTool(&m_github, CCommon::LoadText(IDS_GOTO_GITHUB, _T("\r\nhttps://github.com/zhongyang219/TrafficMonitor")));
	m_tool_tip.AddTool(&m_donate, CCommon::LoadText(IDS_DONATE_ATHOUR));
	m_tool_tip.SetDelayTime(300);	//设置延迟
	m_tool_tip.SetMaxTipWidth(800);

	//设置翻译者信息
	int language_code;
	language_code = _ttoi(CCommon::LoadText(IDS_LANGUAGE_CODE));
	if (language_code == 1 || language_code == 2)		//语言是简体中文和英文时不显示翻译者信息
		m_translaotr_static.ShowWindow(SW_HIDE);
	if (language_code == 3)		//显示繁体中文翻译者的信息
	{
		m_translaotr_static.SetURL(_T("http://mkvq.blogspot.com/"));
		m_tool_tip.AddTool(&m_translaotr_static, CCommon::LoadText(IDS_CONTACT_TRANSLATOR, _T("\r\nhttp://mkvq.blogspot.com/")));
	}

    //设置图片的位置
    CRect rect;
    GetClientRect(rect);
    m_rc_pic = rect;
    ::GetWindowRect(GetDlgItem(IDC_STATIC_VERSION)->GetSafeHwnd(), rect);
    ScreenToClient(rect);
    m_rc_pic.bottom = rect.top - theApp.DPI(6);
    if (m_rc_pic.Height() <= 0)
        m_rc_pic.bottom = m_rc_pic.top + theApp.DPI(50);

    //加载图片
    m_about_pic.LoadBitmap(IDB_ABOUT_BACKGROUND_HD);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

BOOL CAboutDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_MOUSEMOVE)
		m_tool_tip.RelayEvent(pMsg);

	return CDialog::PreTranslateMessage(pMsg);
}

//void CAboutDlg::OnStnClickedStaticDonate()
//{
//	CDonateDlg donateDlg;
//	donateDlg.DoModal();
//}

afx_msg LRESULT CAboutDlg::OnLinkClicked(WPARAM wParam, LPARAM lParam)
{
	CWnd* pCtrl = (CWnd*)wParam;
	if (pCtrl == &m_donate)
	{
		CDonateDlg donateDlg;
		donateDlg.DoModal();
	}
	else if (pCtrl == &m_acknowledgement)
	{
		CString strContent = GetDonateList();
		//strContent += _T("\r\n");
		//strContent += CCommon::LoadText(IDS_ACKNOWLEDGEMENT_EXPLAIN);
		CMessageDlg dlg;
		dlg.SetWindowTitle(CCommon::LoadText(IDS_TITLE_ACKNOWLEDGEMENT));
		//dlg.SetInfoText(CCommon::LoadText(IDS_THANKS_DONORS));
		dlg.SetMessageText(strContent);
		dlg.DoModal();
	}
    else if (pCtrl == &m_license)
    {
        CMessageDlg dlg;
        dlg.SetWindowTitle(CCommon::LoadText(IDS_LICENSE));
        dlg.SetInfoText(CCommon::LoadText(IDS_LICENSE_EXPLAIN));
        dlg.SetMessageText(CCommon::GetTextResource(IDR_LICENSE, 1));
        dlg.DoModal();
    }

	return 0;
}

void CAboutDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CDialog::OnPaint()
    CDrawCommon draw;
    draw.Create(&dc, this);
    draw.GetDC()->FillSolidRect(m_rc_pic, RGB(161, 200, 255));
    draw.DrawBitmap(m_about_pic, m_rc_pic.TopLeft(), m_rc_pic.Size(), CDrawCommon::StretchMode::FIT);
}
