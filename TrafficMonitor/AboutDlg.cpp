#include "stdafx.h"
#include "TrafficMonitor.h"
#include "AboutDlg.h"
#include "MessageDlg.h"
#include "DrawCommon.h"

BEGIN_MESSAGE_MAP(CAboutDlg, CBaseDialog)
    //ON_STN_CLICKED(IDC_STATIC_DONATE, &CAboutDlg::OnStnClickedStaticDonate)
    ON_MESSAGE(WM_LINK_CLICKED, &CAboutDlg::OnLinkClicked)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

CAboutDlg::CAboutDlg() : CBaseDialog(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_MAIL, m_mail);
    DDX_Control(pDX, IDC_STATIC_ACKNOWLEDGEMENT, m_acknowledgement);
    DDX_Control(pDX, IDC_STATIC_GITHUB, m_github);
    DDX_Control(pDX, IDC_STATIC_DONATE, m_donate);
    DDX_Control(pDX, IDC_TRANSLATOR_STATIC, m_translaotr_static);
    DDX_Control(pDX, IDC_STATIC_LICENSE, m_license);
    DDX_Control(pDX, IDC_OPENHARDWAREMONITOR_LINK, m_openhardwaremonitor_link);
    DDX_Control(pDX, IDC_TINYXML2_LINK, m_tinyxml2_link);
    DDX_Control(pDX, IDC_MUSICPLAYER2_LINK, m_musicplayer2_link);
    DDX_Control(pDX, IDC_SIMPLENOTEPAD_LINK, m_simplenotepad_link);
    DDX_Control(pDX, IDC_STATIC_GITEE, m_gitee);
}

CString CAboutDlg::GetDonateList()
{
    return CCommon::GetTextResource(IDR_ACKNOWLEDGEMENT_TEXT, 2);
}

CString CAboutDlg::GetDialogName() const
{
    return CString();
}

BOOL CAboutDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(CCommon::LoadText(IDS_TITLE_ABOUT));
    m_mail.SetURL(_T("mailto:zhongyang219@hotmail.com"));   //设置超链接
    //m_check_update.SetURL(_T("http://pan.baidu.com/s/1c1LkPQ4"));
    m_github.SetURL(_T("https://github.com/zhongyang219/TrafficMonitor"));
    m_gitee.SetURL(_T("https://gitee.com/zhongyang219/TrafficMonitor"));
    m_donate.SetLinkIsURL(false);
    m_acknowledgement.SetLinkIsURL(false);
    m_license.SetLinkIsURL(false);

    m_openhardwaremonitor_link.SetURL(_T("https://github.com/LibreHardwareMonitor/LibreHardwareMonitor"));
    m_tinyxml2_link.SetURL(_T("https://github.com/leethomason/tinyxml2"));
    m_musicplayer2_link.SetURL(_T("https://github.com/zhongyang219/MusicPlayer2"));
    m_simplenotepad_link.SetURL(_T("https://github.com/zhongyang219/SimpleNotePad"));
    m_openhardwaremonitor_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_tinyxml2_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_musicplayer2_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));
    m_simplenotepad_link.SetBackgroundColor(GetSysColor(COLOR_WINDOW));

    //设置版本信息
    CString version_info;
    GetDlgItemText(IDC_STATIC_VERSION, version_info);
    CString str_lite;
#ifdef WITHOUT_TEMPERATURE
    str_lite = CCommon::LoadText(_T(" ("), IDS_WITHOUT_TEMPERATURE, _T(")"));
#endif
    version_info = CCommon::StringFormat(version_info, { str_lite, VERSION });

#ifdef COMPILE_FOR_WINXP
    version_info += _T(" (For WinXP)");
#endif // COMPILE_FOR_WINXP

#ifdef _M_ARM64EC
    version_info += _T(" (Arm64EC)");
#elif _M_X64
    version_info += _T(" (x64)");
#endif

#ifdef _DEBUG
    version_info += _T(" (Debug)");
#endif

    SetDlgItemText(IDC_STATIC_VERSION, version_info);

    //设置最后编译日期
    CString temp_str;
    GetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);
    CString str_compile_time = CCommon::GetLastCompileTime();
    temp_str.Replace(_T("<compile_date>"), str_compile_time);
    SetDlgItemText(IDC_STATIC_COPYRIGHT, temp_str);

    m_tool_tip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
    m_tool_tip.AddTool(&m_mail, CCommon::LoadText(IDS_SEND_EMAIL_TO_ATHOUR, _T("\r\nmailto:zhongyang219@hotmail.com")));
    //m_tool_tip.AddTool(&m_check_update, _T("到百度网盘链接查看是否有更新\r\nhttp://pan.baidu.com/s/1c1LkPQ4"));
    m_tool_tip.AddTool(&m_github, CCommon::LoadText(IDS_GOTO_GITHUB, _T("\r\nhttps://github.com/zhongyang219/TrafficMonitor")));
    m_tool_tip.AddTool(&m_gitee, CCommon::LoadText(IDS_GOTO_GITEE, _T("\r\nhttps://gitee.com/zhongyang219/TrafficMonitor")));
    m_tool_tip.AddTool(&m_donate, CCommon::LoadText(IDS_DONATE_ATHOUR));
    m_tool_tip.AddTool(&m_openhardwaremonitor_link, m_openhardwaremonitor_link.GetURL());
    m_tool_tip.AddTool(&m_tinyxml2_link, m_tinyxml2_link.GetURL());
    m_tool_tip.AddTool(&m_musicplayer2_link, CCommon::LoadText(IDS_MUSICPLAYER2_DESCRIPTION) + _T("\r\n") + m_musicplayer2_link.GetURL());
    m_tool_tip.AddTool(&m_simplenotepad_link, CCommon::LoadText(IDS_SIMPLENOTEPAD_DESCRIPTION) + _T("\r\n") + m_simplenotepad_link.GetURL());

    m_tool_tip.SetDelayTime(300);   //设置延迟
    m_tool_tip.SetMaxTipWidth(800);

    //设置翻译者信息
    int language_code;
    language_code = _ttoi(CCommon::LoadText(IDS_LANGUAGE_CODE));
    if (language_code == 1 || language_code == 2)       //语言是简体中文和英文时不显示翻译者信息
        m_translaotr_static.ShowWindow(SW_HIDE);
    if (language_code == 3)     //显示繁体中文翻译者的信息
    {
        m_translaotr_static.SetURL(_T("http://mkvq.blogspot.com/"));
        m_tool_tip.AddTool(&m_translaotr_static, CCommon::LoadText(IDS_CONTACT_TRANSLATOR, _T("\r\nhttp://mkvq.blogspot.com/")));
    }
    m_translaotr_static.SetBackgroundColor(GetSysColor(COLOR_WINDOW));

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

    return CBaseDialog::PreTranslateMessage(pMsg);
}

//void CAboutDlg::OnStnClickedStaticDonate()
//{
//  CDonateDlg donateDlg;
//  donateDlg.DoModal();
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
                       // 不为绘图消息调用 CBaseDialog::OnPaint()
    CDrawCommon draw;
    draw.Create(&dc, this);
    draw.GetDC()->FillSolidRect(m_rc_pic, RGB(161, 200, 255));
    draw.DrawBitmap(m_about_pic, m_rc_pic.TopLeft(), m_rc_pic.Size(), CDrawCommon::StretchMode::FIT);
}


BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect draw_rect;
    GetClientRect(draw_rect);
    pDC->FillSolidRect(draw_rect, GetSysColor(COLOR_WINDOW));

    //绘制白色背景
    int white_height;       //白色区域的高度
    CRect rc_copyright{};
    ::GetWindowRect(GetDlgItem(IDOK)->GetSafeHwnd(), rc_copyright);
    ScreenToClient(rc_copyright);
    white_height = rc_copyright.top - theApp.DPI(6);

    //绘制“确定”按钮上方的分割线
    CRect rc_line{ draw_rect };
    rc_line.top = white_height;
    rc_line.bottom = white_height + theApp.DPI(1);
    pDC->FillSolidRect(rc_line, RGB(210, 210, 210));

    //绘制灰色背景
    CRect rc_gray{ rc_line };
    rc_gray.top = rc_line.bottom;
    rc_gray.bottom = draw_rect.bottom;
    pDC->FillSolidRect(rc_gray, GetSysColor(COLOR_BTNFACE));

    return TRUE;
    //return CBaseDialog::OnEraseBkgnd(pDC);
}


HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CBaseDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  在此更改 DC 的任何特性
    //去掉static控件的灰色灰色背景
    UINT ctrl_id = pWnd->GetDlgCtrlID();
    if (ctrl_id == IDC_STATIC_VERSION || ctrl_id == IDC_STATIC_COPYRIGHT || ctrl_id == IDC_STATIC)
    {
        static HBRUSH hBackBrush{};
        if (hBackBrush == NULL)
            hBackBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
        pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
        return hBackBrush;
    }

    // TODO:  如果默认的不是所需画笔，则返回另一个画笔
    return hbr;
}
