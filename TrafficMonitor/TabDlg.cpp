#include "stdafx.h"
#include "TabDlg.h"
#include "TrafficMonitor.h"

IMPLEMENT_DYNAMIC(CTabDlg, CBaseDialog)

CTabDlg::CTabDlg(UINT nIDTemplate, CWnd * pParent) : CBaseDialog(nIDTemplate, pParent)
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

    return CBaseDialog::PreTranslateMessage(pMsg);
}


BOOL CTabDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //将窗口背景设置成白色
    if (theApp.m_win_version.IsWindows11OrLater())
        SetBackgroundColor(RGB(249, 249, 249));
    else
        SetBackgroundColor(RGB(255, 255, 255));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

CWnd* CTabDlg::GetParentWindow()
{
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
    {
        return pParent->GetParent();
    }
    return nullptr;
}

void CTabDlg::SetScrollbarInfo(int nPage, int nMax)
{
    // 获取滚动条状态
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    if (m_scroll_enable)
        m_last_pos = scrollinfo.nPos - scrollinfo.nMin;     // 如果已启用滚动则记录当前滚动位置
    else
        m_scroll_enable = true;                             // 否则启用滚动
    int reset_pos = m_last_pos;
    // 父窗口OnSize后的新滚动状态边界检查
    if (nMax < 0) nMax = 0;
    if (m_last_pos > nMax - nPage) m_last_pos = nMax - nPage;
    if (m_last_pos < 0) m_last_pos = 0;
    // 设置滚动条状态
    scrollinfo.nMin = 0;                // 设置滚动内容起点在窗口中的pos
    scrollinfo.nMax = nMax;             // 设置滚动内容终点在窗口中的pos
    scrollinfo.nPage = nPage;           // 设置实际显示内容区域高度
    scrollinfo.nPos = m_last_pos;       // 设置当前实际显示内容区域顶端pos
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    // 复位滚动
    if (reset_pos != 0)
        ScrollWindow(0, reset_pos);
    // 代替框架做出OnSize的响应 (在SetScrollInfo决定并设置滚动条是否在新状态下显示之后)
    CBaseDialog::ResizeDynamicLayout(); // 这个方法会无视滚动更新控件位置导致控件错位，在滚动为0时调用避开这个问题
    // 执行滚动
    if (m_last_pos != 0)
        ScrollWindow(0, -m_last_pos);
}

void CTabDlg::ScrollWindowSimple(int step, bool absolute)
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    if (step == INT_MAX)
        step = scrollinfo.nPage;
    if (step == INT_MIN)
        step = -static_cast<int>(scrollinfo.nPage);
    int offset_diff = scrollinfo.nMax - static_cast<int>(scrollinfo.nPage);
    if (offset_diff < 0)    //nMax应该总是大于或等于nPage，当nMax等于nPage时没有滚动条
        offset_diff = 0;
    if (!absolute)
        scrollinfo.nPos -= step;
    else
    {
        if (step < 0)               // 绝对位置时step为-1表示滚动到底部
            step = offset_diff;
        scrollinfo.nPos = step;     // 此时的stpe为目标绝对位置
        step = m_last_pos - step;   // 将参数的绝对位置转换为偏移量继续处理
    }
    if (scrollinfo.nPos < scrollinfo.nMin)                                          // 限制nPos不小于最小值nMin
    {
        step -= scrollinfo.nMin - scrollinfo.nPos;
        scrollinfo.nPos = scrollinfo.nMin;
    }
    if (scrollinfo.nPos > offset_diff)     // 限制nPos不大于最大值nMax - nPage
    {
        step += scrollinfo.nPos - offset_diff;
        scrollinfo.nPos = offset_diff;
    }
    SetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    m_last_pos = scrollinfo.nPos;
    ScrollWindow(0, step);
}

void CTabDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}

void CTabDlg::ShowDlgCtrl(UINT id, bool show)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->ShowWindow(show);
}

BEGIN_MESSAGE_MAP(CTabDlg, CBaseDialog)
    ON_WM_VSCROLL()
    ON_WM_MOUSEWHEEL()
    ON_WM_SIZE()
END_MESSAGE_MAP()


void CTabDlg::ResizeDynamicLayout()
{
    // m_pDynamicLayout->Adjust()会无视窗口滚动更新控件位置
    // 之前在父窗口OnSize后重置不可靠（进入退出最大化和贴靠时没能正常工作），也没找到检出这两种特殊OnSize的方法
    // 框架内部调用ResizeDynamicLayout太早，来不及先复位滚动，故覆写此虚方法介入

    // 如果已启用滚动那么此处不调用基类使框架的处理无效，改为在父窗口的OnSize后自行调用
    if (!m_scroll_enable)
        CBaseDialog::ResizeDynamicLayout();
}

void CTabDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //参考资料：https://www.cnblogs.com/ranjiewen/p/6013922.html

    if (m_scroll_enable)
    {
        int step = theApp.DPI(16);
        switch (nSBCode)
        {
        case SB_LINEUP:                         //Scroll one line up
            ScrollWindowSimple(step);
            break;
        case SB_LINEDOWN:                       //Scroll one line down
            ScrollWindowSimple(-step);
            break;
        case SB_PAGEUP:                         //Scroll one page up.
            ScrollWindowSimple(INT_MAX);
            break;
        case SB_PAGEDOWN:                       //Scroll one page down
            ScrollWindowSimple(INT_MIN);
            break;
        case SB_THUMBPOSITION:                  //Scroll to the absolute position. The current position is provided in nPos
            break;
        case SB_THUMBTRACK:                     //Drag scroll box to specified position. The current position is provided in nPos
            ScrollWindowSimple(nPos, true);
            break;
        case SB_TOP:                            // 顶部
            ScrollWindowSimple(0, true);
            break;
        case SB_BOTTOM:                         // 底部
            ScrollWindowSimple(-1, true);
            break;
        case SB_ENDSCROLL:                      //End scroll
            break;
        default:
            break;
        }
    }
    CBaseDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CTabDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (m_scroll_enable)
    {
        int step = theApp.DPI(64) * zDelta / 120;
        ScrollWindowSimple(step);
        SCROLLINFO scrollinfo;
        GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
        m_last_pos = scrollinfo.nPos;
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CTabDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    CBaseDialog::OnOK();
}


bool CTabDlg::IsScrollBarVisible()
{
    SCROLLINFO scrollinfo;
    GetScrollInfo(SB_VERT, &scrollinfo, SIF_ALL);
    bool visible = scrollinfo.nPage < scrollinfo.nMax;
    return visible;
}

void CTabDlg::OnSize(UINT nType, int cx, int cy)
{
    CBaseDialog::OnSize(nType, cx, cy);

    // TODO: 在此处添加消息处理程序代码
    SetControlMouseWheelEnable(!IsScrollBarVisible());      //如果显示了滚动条，则禁止控件响应鼠标滚轮，此时鼠标滚轮用于滚动窗口
}
