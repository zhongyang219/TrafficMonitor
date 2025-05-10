// SpinEdit.cpp: 实现文件
//

#include "stdafx.h"
#include "SpinEdit.h"

#define SPIN_ID 3100
// CSpinEdit

IMPLEMENT_DYNAMIC(CSpinEdit, CEdit)

CSpinEdit::CSpinEdit()
{

}

CSpinEdit::~CSpinEdit()
{
}

void CSpinEdit::SetRange(short lower, short upper, short step)
{
    m_step = step;
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetRange(lower, upper);
}

void CSpinEdit::SetValue(int value)
{
    ASSERT(m_step != 0);    // 应当先设置Range
    if (m_spin.GetSafeHwnd() != NULL)
        m_spin.SetPos(value);
}

int CSpinEdit::GetValue()
{
    if (m_spin.GetSafeHwnd() != NULL)
        return m_spin.GetPos32();
    else
        return 0;
}


void CSpinEdit::SetMouseWheelEnable(bool enable)
{
    m_mouse_wheel_enable = enable;
}


BEGIN_MESSAGE_MAP(CSpinEdit, CEdit)
    ON_MESSAGE(WM_TABLET_QUERYSYSTEMGESTURESTATUS, &CSpinEdit::OnTabletQuerysystemgesturestatus)
    ON_NOTIFY(UDN_DELTAPOS, SPIN_ID, &CSpinEdit::OnDeltaposSpin)
    ON_WM_KILLFOCUS()
    ON_WM_ENABLE()
    ON_WM_SIZE()
END_MESSAGE_MAP()



// CSpinEdit 消息处理程序

void CSpinEdit::PreSubclassWindow()
{
    // TODO: 在此添加专用代码和/或调用基类

    // 要求文本左对齐，右侧会被spin控件覆盖
    // WS_CLIPCHILDREN在绘制中排除子窗口区域，避免重叠的控件绘制闪烁
    ModifyStyle(ES_CENTER | ES_RIGHT, ES_LEFT | WS_CLIPCHILDREN);
    m_spin.Create(WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS | UDS_HOTTRACK | UDS_SETBUDDYINT, CRect(), this, SPIN_ID);
    // 获取spin控件宽度
    CRect rect;
    m_spin.GetWindowRect(&rect);
    m_spin_width = rect.Width();
    // 设置spin控件区域（作为CEdit的子控件，ClientRect区域外的部分会被裁剪）
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
    // 设置Edit控件为关联控件
    m_spin.SetBuddy(this);
    // 设置默认范围
    m_spin.SetRange(0, 999);
    CEdit::PreSubclassWindow();
}


BOOL CSpinEdit::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类

    //如果m_mouse_wheel_enable为false，则不响应鼠标滚轮消息
    if (pMsg->message == WM_MOUSEWHEEL && !m_mouse_wheel_enable)
    {
        //将鼠标滚轮消息发送给父窗口
        CWnd* pParent = GetParent();
        pParent->SendMessage(WM_MOUSEWHEEL, pMsg->wParam, pMsg->lParam);
        return true;
    }

    return CEdit::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CSpinEdit::OnTabletQuerysystemgesturestatus(WPARAM wParam, LPARAM lParam)
{
    return 0;
}


void CSpinEdit::OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
    //这里响应微调按钮（spin button）点击上下按钮时的事件
    LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

    int value = GetValue();
    value += pNMUpDown->iDelta * m_step;
    value /= m_step;
    value *= m_step;
    SetValue(value);
    pNMUpDown->iDelta = 0;
    *pResult = 0;
    CWnd* pParent = GetParent();
    if (pParent != nullptr)
        pParent->SendMessage(WM_SPIN_EDIT_POS_CHANGED, (WPARAM)this, (LPARAM)pNMUpDown);
}


void CSpinEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    // TODO: 在此处添加消息处理程序代码
    CString str;
    GetWindowTextW(str);
    int value = _ttoi(str.GetString());
    SetValue(value);    // m_spin会检查并限制值范围以及更新CEdit控件文本
}


void CSpinEdit::OnEnable(BOOL bEnable)
{
    CEdit::OnEnable(bEnable);
    m_spin.EnableWindow(bEnable);
}


void CSpinEdit::OnSize(UINT nType, int cx, int cy)
{
    CEdit::OnSize(nType, cx, cy);
    CRect rect;
    GetClientRect(&rect);
    rect.left = rect.right - m_spin_width;
    m_spin.MoveWindow(&rect);
}
