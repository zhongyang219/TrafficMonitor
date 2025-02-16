#include "stdafx.h"
#include "ClassicalTaskbarDlg.h"

void CClassicalTaskbarDlg::AdjustTaskbarWndPos(bool force_adjust)
{
    ::GetWindowRect(m_hMin, m_rcMin); //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, m_rcBar); //获得二级容器的区域

    static bool last_taskbar_on_top_or_bottom;
    CheckTaskbarOnTopOrBottom();
    if (m_taskbar_on_top_or_bottom != last_taskbar_on_top_or_bottom)
    {
        CalculateWindowSize();
        last_taskbar_on_top_or_bottom = m_taskbar_on_top_or_bottom;
    }

    if (m_taskbar_on_top_or_bottom)     //当任务栏在桌面顶部或底部时
    {
        //设置窗口大小
        m_rect.right = m_rect.left + m_window_width;
        m_rect.bottom = m_rect.top + m_window_height;
        if (force_adjust || m_rcMin.Width() != m_min_bar_width)   //如果最小化窗口的宽度改变了，重新设置任务栏窗口的位置
        {
            m_rcMinOri = m_rcMin;
            m_left_space = m_rcMin.left - m_rcBar.left;
            m_min_bar_width = m_rcMin.Width() - m_rect.Width(); //保存最小化窗口宽度
            //任务窗口显示在右侧时
            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);    //设置最小化窗口的位置
                m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
            }
            //任务栏窗口显示在左侧时
            else
            {
                ::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
                m_rect.MoveToX(m_left_space);
            }

            //设置任务栏窗口的垂直位置
            m_rect.MoveToY((m_rcBar.Height() - m_rect.Height()) / 2);
            if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
                m_rect.MoveToY(m_rect.top + DPI(1));
            MoveWindow(m_rect);
        }
    }
    else        //当任务栏在屏幕在左侧或右侧时
    {
        //设置窗口大小
        if (m_rcMin.Height() != m_min_bar_height) //如果最小化窗口的高度改变了，重新设置任务栏窗口的位置
        {
            m_rcMinOri = m_rcMin;
            m_top_space = m_rcMin.top - m_rcBar.top;
            m_min_bar_height = m_rcMin.Height() - m_rect.Height();  //保存最小化窗口高度
            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                ::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);    //设置最小化窗口的位置
                m_rect.MoveToY(m_top_space + m_rcMin.Height() - m_rect.Height() + 2);
            }
            else
            {
                ::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);  //设置最小化窗口的位置
                m_rect.MoveToY(m_top_space);
            }
            m_rect.MoveToX((m_rcMin.Width() - m_window_width) / 2);
            int left_space = DPI(2);
            if (m_rect.left < left_space)
                m_rect.MoveToX(left_space);
            MoveWindow(m_rect);
        }
    }

    CRect rect{ m_rect };
    //如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
    if (m_connot_insert_to_task_bar)
    {
        rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
        this->MoveWindow(rect);
    }

    if (m_connot_insert_to_task_bar && ::GetForegroundWindow() == m_hTaskbar)   //在窗口无法嵌入任务栏时，如果焦点设置在了任务栏上，则让窗口置顶
    {
        SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);         //设置置顶
    }
}

void CClassicalTaskbarDlg::InitTaskbarWnd()
{
    m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL); //寻找二级容器的句柄
    if (m_hBar == NULL)
        m_hBar = ::FindWindowEx(m_hTaskbar, nullptr, L"WorkerW", NULL);
    m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);    //寻找最小化窗口的句柄
    if (m_hMin == NULL)
        m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskListWClass", NULL);    //寻找最小化窗口的句柄

    ::GetWindowRect(m_hMin, m_rcMin);   //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, m_rcBar);   //获得二级容器的区域

    m_left_space = m_rcMin.left - m_rcBar.left;
    m_top_space = m_rcMin.top - m_rcBar.top;
}

void CClassicalTaskbarDlg::UnInitTaskbarWnd()
{
    //程序关闭的时候，把最小化窗口的width恢复回去
    CheckTaskbarOnTopOrBottom();
    if (m_taskbar_on_top_or_bottom)
        ::MoveWindow(m_hMin, m_left_space, 0, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);
    else

        ::MoveWindow(m_hMin, 0, m_top_space, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);
}

HWND CClassicalTaskbarDlg::GetParentHwnd()
{
    return m_hBar;
}

void CClassicalTaskbarDlg::CheckTaskbarOnTopOrBottom()
{
    CRect rect;
    CRect rcMin;
    CRect rcBar;
    if (m_hTaskbar != 0)
    {
        ::GetWindowRect(m_hMin, rcMin); //获得最小化窗口的区域
        ::GetWindowRect(m_hBar, rcBar); //获得二级容器的区域
        if (m_left_space == 0)
            m_left_space = rcMin.left - rcBar.left;
        if (m_top_space == 0)
            m_top_space = rcMin.top - rcBar.top;

        ::GetWindowRect(m_hTaskbar, rect);          //获取任务栏的矩形区域
        m_taskbar_on_top_or_bottom = (rect.Width() >= rect.Height());     //如果任务栏的宽度大于高度，则任务在屏幕的顶部或底部
    }
    else
    {
        m_taskbar_on_top_or_bottom = true;
    }
}

