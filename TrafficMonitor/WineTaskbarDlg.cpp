#include "stdafx.h"
#include "WineTaskbarDlg.h"

void CWineTaskbarDlg::InitTaskbarWnd()
{
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CWineTaskbarDlg::AdjustTaskbarWndPos(bool force_adjust)
{
    //移动到桌面右下角
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    CRect rect{ m_rect };
    if (theApp.m_taskbar_data.tbar_wnd_on_left)
        rect.MoveToXY(0, screen_height - m_rect.Height());
    else
        rect.MoveToXY(screen_width - m_rect.Width(), screen_height - m_rect.Height());
    this->MoveWindow(rect);
}

void CWineTaskbarDlg::ResetTaskbarPos()
{
}

void CWineTaskbarDlg::CheckTaskbarOnTopOrBottom()
{
    m_taskbar_on_top_or_bottom = true;
}

HWND CWineTaskbarDlg::GetParentHwnd()
{
    return GetDesktopWindow()->GetSafeHwnd();
}
