#include "stdafx.h"
#include "Win11TaskbarDlg.h"
#include "WindowsSettingHelper.h"

namespace
{
    //获取窗口相对于指定父窗口的矩形区域。
    //任务栏窗口是任务栏的子窗口，所以判断它是否已经在目标位置时必须使用相对于父窗口的坐标。
    bool GetWindowRectInParent(HWND hWnd, HWND hParent, CRect& rect)
    {
        if (!::IsWindow(hWnd) || !::IsWindow(hParent))
            return false;
        if (!::GetWindowRect(hWnd, rect))
            return false;
        ::MapWindowPoints(HWND_DESKTOP, hParent, reinterpret_cast<LPPOINT>(&rect), 2);
        return true;
    }
}

void CWin11TaskbarDlg::AdjustTaskbarWndPos(bool force_adjust)
{
    //连接/断开显示器、修改分辨率或资源管理器重建任务栏后，通知区域和开始按钮的窗口可能会被重建，
    //这里在句柄失效或已经不在当前任务栏中时重新获取，避免使用错误的句柄得到错误的位置。
    if (!::IsWindow(m_hNotify) || ::GetParent(m_hNotify) != m_hTaskbar)
        m_hNotify = ::FindWindowEx(m_hTaskbar, 0, L"TrayNotifyWnd", NULL);
    if (!::IsWindow(m_hStart) || ::GetParent(m_hStart) != m_hTaskbar)
        m_hStart = ::FindWindowEx(m_hTaskbar, nullptr, L"Start", NULL);

    m_rcNotify.SetRectEmpty();
    m_rcStart.SetRectEmpty();
    ::GetWindowRect(m_hNotify, m_rcNotify);
    ::GetWindowRect(m_hStart, m_rcStart);

    //将通知区域和开始按钮的坐标由屏幕坐标转换为相对于任务栏左上角的坐标
    m_rcNotify.MoveToXY(m_rcNotify.left - m_rcTaskbar.left, m_rcNotify.top - m_rcTaskbar.top);
    m_rcStart.MoveToXY(m_rcStart.left - m_rcTaskbar.left, m_rcStart.top - m_rcTaskbar.top);
    //没有获取到开始按钮的高度时（例如窗口句柄已经失效），使用任务栏的高度计算垂直位置
    if (m_rcStart.Height() <= 0)
        m_rcStart.SetRect(0, 0, 0, m_rcTaskbar.Height());

    //设置窗口大小
    m_rect.right = m_rect.left + m_window_width;
    m_rect.bottom = m_rect.top + m_window_height;

    //任务窗口显示在右侧时，或者Windows11下任务栏左对齐时
    //（Windows11下，如果任务栏设置为左对齐，即使在“任务栏窗口设置”中设置了任务窗口显示在左边，窗口仍然显示在右边）
    if (!theApp.m_taskbar_data.tbar_wnd_on_left || !CWindowsSettingHelper::IsTaskbarCenterAlign())
    {
        //通知区窗口的水平位置
        int notify_x_pos = m_rcNotify.left;
        //没有获取到通知区位置的情况
        if (m_rcNotify.IsRectEmpty())
        {
            //Win11副屏没有通知区窗口，这里使用固定的值（88像素的系统时间区域）
            if (m_is_secondary_display)
                notify_x_pos = m_rcTaskbar.Width() - DPI(88);
            //如果不是副屏，但是仍然没有获取到通知区域的位置，使用配置文件中taskbar_right_space_win11指定的值
            else
                notify_x_pos = m_rcTaskbar.Width() - DPI(theApp.m_taskbar_data.taskbar_right_space_win11);
        }
        //如果显示了小组件，并且任务栏靠左显示，则留出小组件的位置
        if (theApp.m_taskbar_data.avoid_overlap_with_widgets && CWindowsSettingHelper::IsTaskbarWidgetsBtnShown() && !CWindowsSettingHelper::IsTaskbarCenterAlign())
            m_rect.MoveToX(notify_x_pos - m_rect.Width() + 2 - DPI(theApp.m_taskbar_data.taskbar_left_space_win11));
        else
            m_rect.MoveToX(notify_x_pos - m_rect.Width() + 2);
    }
    //任务栏窗口显示在左侧时
    else
    {
        //靠近“开始”按钮
        if (theApp.m_taskbar_data.tbar_wnd_snap)
        {
            m_rect.MoveToX(m_rcStart.left - m_rect.Width() - 2);
        }
        //靠近最左侧
        else
        {
            if (CWindowsSettingHelper::IsTaskbarWidgetsBtnShown())
                m_rect.MoveToX(2 + DPI(theApp.m_taskbar_data.taskbar_left_space_win11));
            else
                m_rect.MoveToX(2);
        }
    }
    //水平偏移
    m_rect.MoveToX(m_rect.left + DPI(theApp.m_taskbar_data.window_offset_left));
    ////确保水平方向不超出屏幕边界
    //if (m_rect.left < 0)
    //    m_rect.MoveToX(0);
    //if (m_rcTaskbar.Width() > m_rect.Width() && m_rect.right > m_rcTaskbar.Width())
    //    m_rect.MoveToX(m_rcTaskbar.Width() - m_rect.Width());

    //设置任务栏窗口的垂直位置
    //注：这里加上(m_rcTaskbar.Height() - rcStart.Height())用于修正Windows11 build 22621版本后触屏设备任务栏窗口位置不正确的问题。
    //在这种情况下m_rcTaskbar的高度要大于m_rcBar的高度，正常情况下，它们的高度相同
    //但是当任务栏上没有任何图标时，m_rcBar的高度会变为0，因此使用rcStart代替
    m_rect.MoveToY((m_rcStart.Height() - m_rect.Height()) / 2 + (m_rcTaskbar.Height() - m_rcStart.Height()) + DPI(theApp.m_taskbar_data.window_offset_top));
    ////确保垂直方向不超出屏幕边界
    //if (m_rect.top < 0)
    //    m_rect.MoveToY(0);
    //if (m_rcTaskbar.Height() > m_rect.Height() && m_rect.bottom > m_rcTaskbar.Height())
    //    m_rect.MoveToY(m_rcTaskbar.Height() - m_rect.Height());

    //这里不能只在通知区域宽度或开始按钮位置发生变化时才移动窗口。
    //当连接/断开显示器或者修改分辨率时，任务栏的宽度会发生变化，而通知区域的宽度和开始按钮相对于任务栏的位置
    //可能保持不变，此时按原来的判断条件不会重新设置窗口位置，导致任务栏窗口停留在原来的位置而和图标重叠。
    //因此每次都用最新计算出的位置和窗口当前的实际位置比较，只有在不一致时才移动窗口，这样既不会增加多余的重绘，
    //也能保证任务栏窗口始终跟随任务栏的布局。
    CRect current_rect;
    if (force_adjust
        || !GetWindowRectInParent(GetSafeHwnd(), GetParentHwnd(), current_rect)
        || current_rect != m_rect)
    {
        MoveWindow(m_rect);
    }
}

void CWin11TaskbarDlg::InitTaskbarWnd()
{
    m_hNotify = ::FindWindowEx(m_hTaskbar, 0, L"TrayNotifyWnd", NULL);
    m_hStart = ::FindWindowEx(m_hTaskbar, nullptr, L"Start", NULL);
    ::GetWindowRect(m_hNotify, m_rcNotify);
}

void CWin11TaskbarDlg::ResetTaskbarPos()
{
}

HWND CWin11TaskbarDlg::GetParentHwnd()
{
    return m_hTaskbar;
}

void CWin11TaskbarDlg::CheckTaskbarOnTopOrBottom()
{
    m_taskbar_on_top_or_bottom = true;
}
