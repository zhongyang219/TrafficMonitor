// TaskBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"
#include "TrafficMonitorDlg.h"
#include "WindowsSettingHelper.h"


// CTaskBarDlg 对话框

IMPLEMENT_DYNAMIC(CTaskBarDlg, CDialogEx)

CTaskBarDlg::CTaskBarDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_TASK_BAR_DIALOG, pParent)
{

}

CTaskBarDlg::~CTaskBarDlg()
{
    for (auto iter = m_map_history_data.begin(); iter != m_map_history_data.end(); ++iter)
    {
        iter->second.clear();
    }
}

void CTaskBarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTaskBarDlg, CDialogEx)
    ON_WM_RBUTTONUP()
    ON_WM_INITMENU()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_TIMER()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    ON_WM_LBUTTONUP()
    ON_MESSAGE(WM_EXITMENULOOP, &CTaskBarDlg::OnExitmenuloop)
END_MESSAGE_MAP()


// CTaskBarDlg 消息处理程序


void CTaskBarDlg::ShowInfo(CDC* pDC)
{
    if (this->GetSafeHwnd() == NULL || pDC == nullptr || !IsWindow(this->GetSafeHwnd())) return;

    if (m_rect.IsRectEmpty() || m_rect.IsRectNull()) return;
    CRect draw_rect{ m_rect };      //绘图的矩形区域
    draw_rect.MoveToXY(0, 0);
    //设置缓冲的DC
    CDrawDoubleBuffer draw_double_buffer(pDC, draw_rect);
    //绘图
    CDrawCommon draw;
    draw.Create(draw_double_buffer.GetMemDC(), nullptr);
    draw.FillRect(draw_rect, theApp.m_taskbar_data.back_color);       //填充背景色
    draw.SetFont(&m_font);
    draw.SetBackColor(theApp.m_taskbar_data.back_color);

    //计算各部分的位置
    int index = 0;
    CRect item_rect{};
    int item_count = static_cast<int>(m_item_widths.size());  //要显示的项目数量
    auto last_iter = m_item_widths.begin();
    for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
    {
        auto last_item_width = last_iter->item_width;
        //任务栏在桌面顶部或底部
        if (IsTasksbarOnTopOrBottom())
        {
            if (theApp.m_taskbar_data.horizontal_arrange)   //水平排列
            {
                if (index > 0)
                    item_rect.MoveToX(item_rect.right + theApp.DPI(theApp.m_taskbar_data.item_space));
                item_rect.right = item_rect.left + iter->item_width.TotalWidth();
                item_rect.bottom = item_rect.top + m_window_height;
                if (iter->is_plugin)
                    DrawPluginItem(draw, iter->plugin_item, item_rect, iter->item_width.label_width);
                else
                    DrawDisplayItem(draw, iter->item_type, item_rect, iter->item_width.label_width);
            }
            else        //非水平排列时，每两个一组显示
            {
                //在index为奇数时同时绘制两个项目
                if (index % 2 == 1)
                {
                    CRect item_rect_up;     //上面一个项目的矩形区域
                    if (index > 0)
                        item_rect_up.MoveToXY(item_rect.right + theApp.DPI(theApp.m_taskbar_data.item_space), 0);
                    item_rect.left = item_rect_up.left;
                    item_rect.top = (m_window_height - TASKBAR_WND_HEIGHT / 2);
                    //确定窗口大小
                    item_rect_up.bottom = item_rect.top - 1;
                    item_rect.bottom = m_window_height;
                    int width = max(iter->item_width.TotalWidth(), last_item_width.TotalWidth());
                    item_rect.right = item_rect.left + width;
                    item_rect_up.right = item_rect_up.left + width;
                    //绘制信息
                    if (last_iter->is_plugin)
                        DrawPluginItem(draw, last_iter->plugin_item, item_rect_up, last_item_width.label_width);
                    else
                        DrawDisplayItem(draw, last_iter->item_type, item_rect_up, last_item_width.label_width);

                    if (iter->is_plugin)
                        DrawPluginItem(draw, iter->plugin_item, item_rect, iter->item_width.label_width);
                    else
                        DrawDisplayItem(draw, iter->item_type, item_rect, iter->item_width.label_width);
                }
                //要绘制的项目为奇数时绘制最后一个
                else if (item_count % 2 == 1 && index == item_count - 1)
                {
                    item_rect.MoveToXY(item_rect.right + theApp.DPI(theApp.m_taskbar_data.item_space), 0);
                    item_rect.bottom = TASKBAR_WND_HEIGHT;
                    item_rect.right = item_rect.left + iter->item_width.MaxWidth();
                    if (iter->is_plugin)
                        DrawPluginItem(draw, iter->plugin_item, item_rect, iter->item_width.label_width, true);
                    else
                        DrawDisplayItem(draw, iter->item_type, item_rect, iter->item_width.label_width, true);
                }
            }
        }
        //任务栏在桌面两侧
        else
        {
            if (index > 0)
                item_rect.MoveToXY(0, item_rect.bottom + theApp.DPI(theApp.m_taskbar_data.item_space));
            item_rect.bottom = item_rect.top + TASKBAR_WND_HEIGHT / 2;
            item_rect.right = item_rect.left + min(m_window_width, m_rcMin.Width() - theApp.DPI(theApp.m_taskbar_data.item_space));
            if (iter->is_plugin)
                DrawPluginItem(draw, iter->plugin_item, item_rect, iter->item_width.label_width);
            else
                DrawDisplayItem(draw, iter->item_type, item_rect, iter->item_width.label_width);
        }

        index++;
        last_iter = iter;
    }
}

void CTaskBarDlg::DrawDisplayItem(CDrawCommon& drawer, DisplayItem type, CRect rect, int label_width, bool vertical)
{
    m_item_rects[type] = rect;
    //设置要绘制的文本颜色
    COLORREF label_color{};
    COLORREF text_color{};
    if (theApp.m_taskbar_data.specify_each_item_color)
    {
        label_color = theApp.m_taskbar_data.text_colors[type].label;
        text_color = theApp.m_taskbar_data.text_colors[type].value;
    }
    else if (!theApp.m_taskbar_data.text_colors.empty())
    {
        label_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
        text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
    }

    //设置标签和数值的矩形区域
    CRect rect_label{ rect };
    CRect rect_value{ rect };
    if (!vertical)
    {
        rect_label.right = rect_label.left + label_width;
        rect_value.left = rect_label.right;
    }
    else if (label_width != 0)
    {
        rect_label.bottom = rect_label.top + (rect_label.Height() / 2);
        rect_value.top = rect_label.bottom;
    }

    // 绘制状态条
    if (type == TDI_CPU || type == TDI_MEMORY || type == TDI_GPU_USAGE || type == TDI_CPU_TEMP
        || type == TDI_GPU_TEMP || type == TDI_HDD_TEMP || type == TDI_MAIN_BOARD_TEMP || type == TDI_HDD_USAGE
        || type == TDI_UP || type == TDI_DOWN || type == TDI_TOTAL_SPEED/* ||type==TDI_CPU_FREQ*/)
    {
        int figure_value{};
        switch (type)
        {
        case TDI_CPU:
            figure_value = theApp.m_cpu_usage;
            break;
        case TDI_MEMORY:
            figure_value = theApp.m_memory_usage;
            break;
        case TDI_GPU_USAGE:
            figure_value = theApp.m_gpu_usage;
            break;
        case TDI_CPU_TEMP:
            figure_value = theApp.m_cpu_temperature;
            break;
        case TDI_GPU_TEMP:
            figure_value = theApp.m_gpu_temperature;
            break;
        case TDI_HDD_TEMP:
            figure_value = theApp.m_hdd_temperature;
            break;
        case TDI_MAIN_BOARD_TEMP:
            figure_value = theApp.m_main_board_temperature;
            break;
        case TDI_HDD_USAGE:
            figure_value = theApp.m_hdd_usage;
            break;
        //case TDI_CPU_FREQ:
        //    figure_value = theApp.m_cpu_freq;
        //    break;
        case TDI_UP:
            figure_value = CalculateNetspeedPercent(theApp.m_out_speed);
            break;
        case TDI_DOWN:
            figure_value = CalculateNetspeedPercent(theApp.m_in_speed);
            break;
        case TDI_TOTAL_SPEED:
            figure_value = CalculateNetspeedPercent(theApp.m_in_speed + theApp.m_out_speed);
            break;
        default:
            break;
        }

        if ((type != TDI_UP && type != TDI_DOWN && type != TDI_TOTAL_SPEED) && theApp.m_taskbar_data.show_status_bar
            || (type == TDI_UP || type == TDI_DOWN || type == TDI_TOTAL_SPEED) && theApp.m_taskbar_data.show_netspeed_figure)
        {
            if (theApp.m_taskbar_data.cm_graph_type)
            {
                AddHisToList(type, figure_value);
                TryDrawGraph(drawer, rect, type);
            }
            else
            {
                TryDrawStatusBar(drawer, rect, figure_value);
            }
        }
    }

    //绘制标签
    if (label_width > 0)
    {
        wstring str_label = theApp.m_taskbar_data.disp_str.Get(type);
        //if (theApp.m_taskbar_data.swap_up_down)
        //{
        //    if (type == TDI_UP)
        //        str_label = theApp.m_taskbar_data.disp_str.Get(TDI_DOWN);
        //    else if (type == TDI_DOWN)
        //        str_label = theApp.m_taskbar_data.disp_str.Get(TDI_UP);
        //}
        drawer.DrawWindowText(rect_label, str_label.c_str(), label_color, (vertical ? Alignment::CENTER : Alignment::LEFT));
    }

    //绘制数值
    CString str_value;
    Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? Alignment::RIGHT : Alignment::LEFT };      //数值的对齐方式
    if (vertical)
        value_alignment = Alignment::CENTER;
    //绘制上传或下载速度
    if (type == TDI_UP || type == TDI_DOWN || type == TDI_TOTAL_SPEED)
    {
        CString format_str;
        if (theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO)
            format_str = _T("%s");
        else
            format_str = _T("%s/s");
        CString str_in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_taskbar_data);
        CString str_out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_taskbar_data);
        CString str_total_speed = CCommon::DataSizeToString(theApp.m_in_speed + theApp.m_out_speed, theApp.m_taskbar_data);
        //if (theApp.m_taskbar_data.swap_up_down)
        //    std::swap(str_in_speed, str_out_speed);
        if (type == TDI_UP)
        {
            str_value.Format(format_str, str_out_speed.GetString());
        }
        else if (type == TDI_DOWN)
        {
            str_value.Format(format_str, str_in_speed.GetString());
        }
        else
        {
            str_value.Format(format_str, str_total_speed.GetString());
        }
    }

    //当内存显示为已使用内存或可用内存时
    if (type == TDI_MEMORY && (theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_USED || theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_AVAILABLE))
    {
        if (theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_USED)
            str_value = CCommon::DataSizeToString(static_cast<unsigned long long>(theApp.m_used_memory) * 1024, theApp.m_taskbar_data.separate_value_unit_with_space);
        else
            str_value = CCommon::DataSizeToString((static_cast<unsigned long long>(theApp.m_total_memory) - static_cast<unsigned long long>(theApp.m_used_memory)) * 1024, theApp.m_taskbar_data.separate_value_unit_with_space);
    }
    //绘制CPU或内存利用率
    else if (type == TDI_CPU || type == TDI_MEMORY || type == TDI_GPU_USAGE || type == TDI_HDD_USAGE)
    {
        int usage{};
        switch (type)
        {
        case TDI_CPU:
            usage = theApp.m_cpu_usage;
            break;
        case TDI_MEMORY:
            usage = theApp.m_memory_usage;
            break;
        case TDI_GPU_USAGE:
            usage = theApp.m_gpu_usage;
            break;
        case TDI_HDD_USAGE:
            usage = theApp.m_hdd_usage;
            break;
        default:
            break;
        }
        str_value = CCommon::UsageToString(usage, theApp.m_taskbar_data);

        //如果CPU或内存利用率达到100%，会导致显示不全，此时将绘图区域向右扩展一些
        int text_width = m_pDC->GetTextExtent(str_value).cx;
        if (usage >= 100 && rect_value.Width() < text_width)
            rect_value.right = rect_value.left + text_width;
    }

    //绘制温度
    else if (type == TDI_CPU_TEMP || type == TDI_GPU_TEMP || type == TDI_HDD_TEMP || type == TDI_MAIN_BOARD_TEMP)
    {
        int temperature{};
        switch (type)
        {
        case TDI_CPU_TEMP:
            temperature = theApp.m_cpu_temperature;
            break;
        case TDI_GPU_TEMP:
            temperature = theApp.m_gpu_temperature;
            break;
        case TDI_HDD_TEMP:
            temperature = theApp.m_hdd_temperature;
            break;
        case TDI_MAIN_BOARD_TEMP:
            temperature = theApp.m_main_board_temperature;
            break;
        default:
            break;
        }
        str_value = CCommon::TemperatureToString(temperature, theApp.m_taskbar_data);
    }
    else if (type == TDI_CPU_FREQ) {
        str_value = CCommon::FreqToString(theApp.m_cpu_freq, theApp.m_taskbar_data);
    }

    drawer.DrawWindowText(rect_value, str_value, text_color, value_alignment);
}

void CTaskBarDlg::DrawPluginItem(CDrawCommon& drawer, IPluginItem* item, CRect rect, int label_width, bool vertical)
{
    if (item == nullptr)
        return;
    m_item_rects[item] = rect;
    //设置要绘制的文本颜色
    COLORREF label_text_color{};
    COLORREF value_text_color{};
    if (theApp.m_taskbar_data.specify_each_item_color)
    {
        label_text_color = theApp.m_taskbar_data.text_colors[item].label;
        value_text_color = theApp.m_taskbar_data.text_colors[item].value;
    }
    else if (!theApp.m_taskbar_data.text_colors.empty())
    {
        label_text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
        value_text_color = theApp.m_taskbar_data.text_colors.begin()->second.label;
    }

    if (item->IsCustomDraw())
    {
        //根据背景色的亮度判断深色还是浅色模式
        const COLORREF& bk{ theApp.m_taskbar_data.back_color };
        int background_brightness{ (GetRValue(bk) + GetGValue(bk) + GetBValue(bk)) / 3 };
        //由插件自绘
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(item);
        if (plugin != nullptr && plugin->GetAPIVersion() >= 2)
        {
            plugin->OnExtenedInfo(ITMPlugin::EI_LABEL_TEXT_COLOR, std::to_wstring(label_text_color).c_str());
            plugin->OnExtenedInfo(ITMPlugin::EI_VALUE_TEXT_COLOR, std::to_wstring(value_text_color).c_str());
            plugin->OnExtenedInfo(ITMPlugin::EI_DRAW_TASKBAR_WND, L"1");
        }
        drawer.GetDC()->SetTextColor(value_text_color);
        item->DrawItem(drawer.GetDC()->GetSafeHdc(), rect.left, rect.top, rect.Width(), rect.Height(), background_brightness < 128);
    }
    else
    {
        CRect rect_label, rect_value;
        rect_label = rect_value = rect;
        if (label_width > 0)
        {
            if (!vertical)
            {
                rect_label = rect_value = rect;
                rect_label.right = rect_label.left + label_width;
                rect_value.left = rect_label.right;
            }
            else
            {
                rect_label.bottom = rect_label.top + rect.Height() / 2;
                rect_value.top = rect_label.bottom;
            }
        }
        //画标签
        CString lable_text = theApp.m_taskbar_data.disp_str.Get(item).c_str();
        lable_text += L' ';
        drawer.DrawWindowText(rect_label, lable_text, label_text_color, (vertical ? Alignment::CENTER : Alignment::LEFT));
        //画数值
        Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? Alignment::RIGHT : Alignment::LEFT };      //数值的对齐方式
        if (vertical)
            value_alignment = Alignment::CENTER;
        drawer.DrawWindowText(rect_value, item->GetItemValueText(), value_text_color, value_alignment);
    }
}

void CTaskBarDlg::MoveWindow(CRect rect)
{
    if (IsWindow(GetSafeHwnd()))
    {
        ::MoveWindow(GetSafeHwnd(), rect.left, rect.top, rect.Width(), rect.Height(), TRUE);
    }
}

void CTaskBarDlg::TryDrawStatusBar(CDrawCommon& drawer, const CRect& rect_bar, int usage_percent)
{
    CSize fill_size = CSize(rect_bar.Width() * usage_percent / 100, rect_bar.Height());
    CRect rect_fill(rect_bar.TopLeft(), fill_size);
    if (theApp.m_taskbar_data.show_graph_dashed_box)
        drawer.DrawRectOutLine(rect_bar, theApp.m_taskbar_data.status_bar_color, 1, true);
    drawer.FillRect(rect_fill, theApp.m_taskbar_data.status_bar_color);
}

bool CTaskBarDlg::AdjustWindowPos()
{
    if (this->GetSafeHwnd() == NULL || !IsWindow(this->GetSafeHwnd()))
        return false;
    ::GetWindowRect(m_hMin, m_rcMin); //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, m_rcBar); //获得二级容器的区域

    ::GetWindowRect(m_hNotify, m_rcNotify);

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
        if (m_rcMin.Width() != m_min_bar_width)   //如果最小化窗口的宽度改变了，重新设置任务栏窗口的位置
        {
            m_rcMinOri = m_rcMin;
            m_left_space = m_rcMin.left - m_rcBar.left;
            m_min_bar_width = m_rcMin.Width() - m_rect.Width(); //保存最小化窗口宽度
            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                if (theApp.m_is_windows11_taskbar)
                {
                    if (!theApp.m_taskbar_data.tbar_wnd_snap)
                        m_rect.MoveToX(m_rcNotify.left - m_rect.Width() + 2);
                    else
                        m_rect.MoveToX(m_rcMin.right + 2);
                }
                else
                {
                    ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);    //设置最小化窗口的位置
                    m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
                }
            }
            else
            {
                if (theApp.m_is_windows11_taskbar)
                {
                    if (CWindowsSettingHelper::IsTaskbarCenterAlign())
                    {
                        if (theApp.m_taskbar_data.tbar_wnd_snap)
                        {
                            int taskbar_btn_num{ 1 };      //Win11任务栏“运行中的程序”左侧4个按钮（开始、搜索、任务视图、聊天）有几个显示。（“开始”按钮总是显示）
                            if (CWindowsSettingHelper::IsTaskbarSearchBtnShown())
                                taskbar_btn_num++;
                            if (CWindowsSettingHelper::IsTaskbarTaskViewBtnShown())
                                taskbar_btn_num++;
                            if (CWindowsSettingHelper::IsTaskbarChartBtnShown())
                                taskbar_btn_num++;

                            m_rect.MoveToX(m_rcMin.left - m_rect.Width() - 2 - theApp.DPI(44) * taskbar_btn_num);   //每个按钮44像素
                        }
                        else
                        {
                            if (CWindowsSettingHelper::IsTaskbarWidgetsBtnShown())
                                m_rect.MoveToX(2 + theApp.DPI(theApp.m_cfg_data.taskbar_left_space_win11));
                            else
                                m_rect.MoveToX(2);
                        }
                    }
                    else
                    {
                        m_rect.MoveToX(2);
                    }
                }
                else
                {
                    ::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
                    m_rect.MoveToX(m_left_space);
                }
            }
            m_rect.MoveToY((m_rcBar.Height() - m_rect.Height()) / 2);
            if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
                m_rect.MoveToY(m_rect.top + theApp.DPI(1));
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
            if (m_rect.left < theApp.DPI(2))
                m_rect.MoveToX(theApp.DPI(2));
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
    return true;
}

void CTaskBarDlg::ApplyWindowTransparentColor()
{
#ifndef COMPILE_FOR_WINXP
    if (theApp.m_is_windows11_taskbar)      //Windows11下背景色不使用纯黑色，以解决深色模式下右键菜单无法弹出的问题
    {
        if (theApp.m_taskbar_data.transparent_color == 0 && theApp.m_taskbar_data.back_color == 0)
        {
            theApp.m_taskbar_data.transparent_color = 1;
            theApp.m_taskbar_data.back_color = 1;
        }
    }
    if ((theApp.m_taskbar_data.transparent_color != 0) && theApp.m_taksbar_transparent_color_enable)
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(theApp.m_taskbar_data.transparent_color, 0, LWA_COLORKEY);
    }
    else
    {
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
    }
#endif // !COMPILE_FOR_WINXP
}


void CTaskBarDlg::CheckTaskbarOnTopOrBottom()
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

CString CTaskBarDlg::GetMouseTipsInfo()
{
    CString tip_info;
    CString temp;
    temp.Format(_T("%s: %s\r\n (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
        CCommon::KBytesToString((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u),
        CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(theApp.m_today_up_traffic / 1024u),
        CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(theApp.m_today_down_traffic / 1024u)
    );
    tip_info += temp;
    if (!IsItemShow(TDI_UP))
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
            CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!IsItemShow(TDI_DOWN))
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
            CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!IsItemShow(TDI_CPU))
    {
        temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_CPU_USAGE), theApp.m_cpu_usage);
        tip_info += temp;
    }
    if (!IsShowCpuMemory())
    {
        temp.Format(_T("\r\n%s: %s/%s"),
            CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory), CCommon::KBytesToString(theApp.m_total_memory));
    }
    if (!IsItemShow(TDI_MEMORY))
    {
        temp.Format(_T("\r\n%s: %s/%s (%d %%)"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory), theApp.m_memory_usage);
        tip_info += temp;
    }
    else
    {
        temp.Format(_T("\r\n%s: %s/%s"), CCommon::LoadText(IDS_MEMORY_USAGE),
            CCommon::KBytesToString(theApp.m_used_memory),
            CCommon::KBytesToString(theApp.m_total_memory));
        tip_info += temp;
    }
#ifndef WITHOUT_TEMPERATURE
    CTrafficMonitorDlg* pMainWnd = dynamic_cast<CTrafficMonitorDlg*>(theApp.m_pMainWnd);
    if (pMainWnd->IsTemperatureNeeded())
    {
        if (!IsItemShow(TDI_GPU_USAGE) && theApp.m_gpu_usage >= 0)
        {
            temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_GPU_USAGE), theApp.m_gpu_usage);
            tip_info += temp;
        }
        if (!IsItemShow(TDI_CPU_TEMP) && theApp.m_cpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_cpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_CPU_FREQ) && theApp.m_cpu_freq > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_FREQ), CCommon::FreqToString(theApp.m_cpu_freq, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_GPU_TEMP) && theApp.m_gpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_GPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_gpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_HDD_TEMP) && theApp.m_hdd_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_HDD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_hdd_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_MAIN_BOARD_TEMP) && theApp.m_main_board_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_main_board_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsItemShow(TDI_HDD_USAGE) && theApp.m_hdd_usage >= 0)
        {
            temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_HDD_USAGE), theApp.m_hdd_usage);
            tip_info += temp;
        }
    }
#endif

    //添加插件项目的鼠标提示
    tip_info += theApp.GetPlauginTooltipInfo().c_str();

    return tip_info;
}

void CTaskBarDlg::SetTextFont()
{
    //如果m_font已经关联了一个字体资源对象，则释放它
    if (m_font.m_hObject)
    {
        m_font.DeleteObject();
    }
    //创建新的字体
    theApp.m_taskbar_data.font.Create(m_font, theApp.GetDpi());
}

void CTaskBarDlg::ApplySettings()
{
    SetTextFont();
    CalculateWindowSize();
}

void CTaskBarDlg::CalculateWindowSize()
{
    bool horizontal_arrange = theApp.m_taskbar_data.horizontal_arrange && m_taskbar_on_top_or_bottom;
    if (theApp.m_taskbar_data.m_tbar_display_item == 0)
        theApp.m_taskbar_data.m_tbar_display_item |= TDI_UP;        //至少显示一项

    m_item_widths.clear();
    //显示项目的宽度
    std::map<CommonDisplayItem, ItemWidth> item_widths;

    m_pDC->SelectObject(&m_font);
    //计算标签宽度
    //const auto& item_map = theApp.m_taskbar_data.disp_str.GetAllItems();
    for (auto iter = theApp.m_plugins.AllDisplayItemsWithPlugins().begin(); iter != theApp.m_plugins.AllDisplayItemsWithPlugins().end(); ++iter)
    {
        if (iter->is_plugin)
        {
            auto plugin = iter->plugin_item;
            if (plugin != nullptr)
            {
                int& label_width{ item_widths[*iter].label_width };
                if (plugin->IsCustomDraw())
                {
                    label_width = 0;
                }
                else
                {
                    CString lable_text = theApp.m_taskbar_data.disp_str.Get(plugin).c_str();
                    if (!lable_text.IsEmpty())
                        lable_text += L' ';
                    label_width = m_pDC->GetTextExtent(lable_text).cx;
                }
            }
        }
        else
        {
            item_widths[*iter].label_width = m_pDC->GetTextExtent(theApp.m_taskbar_data.disp_str.Get(*iter).c_str()).cx;
        }
    }

    //计算数值部分宽度

    //计算显示上传下载部分所需要的宽度
    CString sample_str;
    int value_width{};
    wstring digits(theApp.m_taskbar_data.digits_number, L'8');      //根据数据位数生成指定个数的“8”
    bool hide_unit{ theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO };
    if (theApp.m_taskbar_data.speed_short_mode)
    {
        if (hide_unit)
            sample_str.Format(_T("%s."), digits.c_str());
        else
            sample_str.Format(_T("%s.M/s"), digits.c_str());
    }
    else
    {
        if (hide_unit)
            sample_str.Format(_T("%s.8"), digits.c_str());
        else
            sample_str.Format(_T("%s.8MB/s"), digits.c_str());
    }
    if (!hide_unit && theApp.m_taskbar_data.separate_value_unit_with_space)
        sample_str += _T(' ');
    if (theApp.m_taskbar_data.speed_short_mode && !theApp.m_taskbar_data.unit_byte && !theApp.m_taskbar_data.hide_unit)
        sample_str += _T('b');
    value_width = m_pDC->GetTextExtent(sample_str).cx;      //计算使用当前字体显示文本需要的宽度值
    item_widths[TDI_UP].value_width = value_width;
    item_widths[TDI_DOWN].value_width = value_width;
    item_widths[TDI_TOTAL_SPEED].value_width = value_width;

    //计算显示CPU、内存部分所需要的宽度
    CString str;
    if (theApp.m_taskbar_data.hide_percent)
    {
        str = _T("99");
    }
    else if (theApp.m_taskbar_data.separate_value_unit_with_space)
    {
        str = _T("99 %");
    }
    else
    {
        str = _T("99%");
    }
    value_width = m_pDC->GetTextExtent(str).cx;
    //内存显示的宽度
    int memory_width{ value_width };
    if (theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_USED || theApp.m_taskbar_data.memory_display == MemoryDisplay::MEMORY_AVAILABLE)
    {
        if (theApp.m_taskbar_data.separate_value_unit_with_space)
            str = _T("19.99 GB");
        else
            str = _T("19.99GB");
        memory_width = m_pDC->GetTextExtent(str).cx;
    }
    item_widths[TDI_CPU].value_width = value_width;
    item_widths[TDI_MEMORY].value_width = memory_width;
    item_widths[TDI_GPU_USAGE].value_width = value_width;
    item_widths[TDI_HDD_USAGE].value_width = value_width;

    item_widths[TDI_CPU_FREQ].value_width = m_pDC->GetTextExtent(_T("1.00 GHz")).cx;

    //计算温度显示的宽度
    if (theApp.m_taskbar_data.separate_value_unit_with_space)
        str = _T("99 °C");
    else
        str = _T("99°C");
    value_width = m_pDC->GetTextExtent(str).cx;
    value_width += theApp.DPI(2);
    item_widths[TDI_CPU_TEMP].value_width = value_width;
    item_widths[TDI_GPU_TEMP].value_width = value_width;
    item_widths[TDI_HDD_TEMP].value_width = value_width;
    item_widths[TDI_MAIN_BOARD_TEMP].value_width = value_width;

    //计算插件项目的宽度
    for (const auto& plugin : theApp.m_plugins.GetPluginItems())
    {
        int& value_width{ item_widths[plugin].value_width };
        if (plugin != nullptr && theApp.m_taskbar_data.plugin_display_item.Contains(plugin->GetItemId()))
        {
            if (plugin->IsCustomDraw())
            {
                value_width = theApp.m_plugins.GetItemWidth(plugin, m_pDC);
            }
            else
            {
                value_width = m_pDC->GetTextExtent(plugin->GetItemValueSampleText()).cx;
            }
        }
    }

    auto item_order{ theApp.m_taskbar_data.item_order.GetAllDisplayItemsWithOrder() };
    for (const auto& item : item_order)
    {
        if (theApp.IsTaksbarItemDisplayed(item))
        {
            ItemWidthInfo width_info = item;
            width_info.item_width = item_widths[item];
            m_item_widths.push_back(width_info);
        }
    }

    int item_count = static_cast<int>(m_item_widths.size());

    //计算窗口总宽度
    if (IsTasksbarOnTopOrBottom())  //任务栏在桌面的顶部或底部时
    {
        m_window_width = 0;
        if (theApp.m_taskbar_data.horizontal_arrange)   //水平排列时
        {
            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                m_window_width += iter->item_width.TotalWidth();
            }
            m_window_width += theApp.DPI(theApp.m_taskbar_data.item_space) * item_count;   //加上每个标签间的空隙
        }
        else        //非水平排列时，每两个一组排列
        {
            int index = 0;
            int width0;
            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                if (index % 2 == 0)
                {
                    width0 = iter->item_width.TotalWidth();
                }
                else
                {
                    m_window_width += max(width0, iter->item_width.TotalWidth());
                }
                if (item_count % 2 == 1 && index == item_count - 1) //项目数为奇数时加上最后一个的宽度
                {
                    m_window_width += iter->item_width.MaxWidth();
                }

                index++;
            }

            m_window_width += theApp.DPI(theApp.m_taskbar_data.item_space) * ((item_count + 1) / 2 + 1);   //加上每个标签间的空隙
        }
    }
    else        //任务栏在桌面两侧时
    {
        m_window_width = 0;
        //所有标签中最大的宽度即为窗口宽度
        for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
        {
            if (m_window_width < iter->item_width.TotalWidth())
                m_window_width = iter->item_width.TotalWidth();
        }
    }

    //计算窗口高度
    if (IsTasksbarOnTopOrBottom())
    {
        if (!horizontal_arrange)
            m_window_height = TASKBAR_WND_HEIGHT;
        else
            m_window_height = TASKBAR_WND_HEIGHT * 2 / 3;
    }
    else
    {
        m_window_height = TASKBAR_WND_HEIGHT / 2 * item_count;
        m_window_height += (theApp.DPI(theApp.m_taskbar_data.item_space) * item_count);   //加上每个标签间的空隙
    }
    m_rect.right = m_rect.left + m_window_width;
    m_rect.bottom = m_rect.top + m_window_height;

}

void CTaskBarDlg::SetToolTipsTopMost()
{
    m_tool_tips.SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void CTaskBarDlg::UpdateToolTips()
{
    if (theApp.m_taskbar_data.show_tool_tip && IsWindow(m_tool_tips.GetSafeHwnd()))
    {
        CString tip_info;
        tip_info = GetMouseTipsInfo();
        m_tool_tips.UpdateTipText(tip_info, this);
    }
}

bool CTaskBarDlg::IsItemShow(DisplayItem item)
{
    return (theApp.m_taskbar_data.m_tbar_display_item & item);
}

bool CTaskBarDlg::IsShowCpuMemory()
{
    return ((theApp.m_taskbar_data.m_tbar_display_item & TDI_CPU) || (theApp.m_taskbar_data.m_tbar_display_item & TDI_MEMORY));
}

bool CTaskBarDlg::IsShowNetSpeed()
{
    return ((theApp.m_taskbar_data.m_tbar_display_item & TDI_UP) || (theApp.m_taskbar_data.m_tbar_display_item & TDI_DOWN));
}


BOOL CTaskBarDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    //设置隐藏任务栏图标
    ModifyStyleEx(0, WS_EX_TOOLWINDOW);

    m_pDC = GetDC();


    //设置字体
    SetTextFont();
    m_pDC->SelectObject(&m_font);


    m_hTaskbar = ::FindWindow(L"Shell_TrayWnd", NULL);      //寻找类名是Shell_TrayWnd的窗口句柄
    m_hBar = ::FindWindowEx(m_hTaskbar, 0, L"ReBarWindow32", NULL); //寻找二级容器的句柄
    m_hMin = ::FindWindowEx(m_hBar, 0, L"MSTaskSwWClass", NULL);    //寻找最小化窗口的句柄
    
    m_hNotify = ::FindWindowEx(m_hTaskbar, 0, L"TrayNotifyWnd", NULL);

    //在“Shell_TrayWnd”的子窗口找到类名为“Windows.UI.Composition.DesktopWindowContentBridge”的窗口则认为是Windows11的任务栏
    if (theApp.m_win_version.IsWindows11OrLater())
    {
        theApp.m_is_windows11_taskbar = (::FindWindowExW(m_hTaskbar, 0, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL) != NULL);
    }
    //设置窗口透明色
    ApplyWindowTransparentColor();

    ::GetWindowRect(m_hMin, m_rcMin);   //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, m_rcBar);   //获得二级容器的区域

    ::GetWindowRect(m_hNotify, m_rcNotify);

    m_left_space = m_rcMin.left - m_rcBar.left;
    m_top_space = m_rcMin.top - m_rcBar.top;

    CheckTaskbarOnTopOrBottom();
    CalculateWindowSize();
    m_rect.SetRectEmpty();
    m_rect.bottom = m_window_height;
    m_rect.right = m_rect.left + m_window_width;

    m_connot_insert_to_task_bar = !(::SetParent(this->m_hWnd, m_hTaskbar)); //把程序窗口设置成任务栏的子窗口
    m_error_code = GetLastError();
    AdjustWindowPos();

    SetBackgroundColor(theApp.m_taskbar_data.back_color);

    //初始化鼠标提示
    if (IsWindow(GetSafeHwnd()) && m_tool_tips.Create(this, TTS_ALWAYSTIP) && IsWindow(m_tool_tips.GetSafeHwnd()))
    {
        m_tool_tips.SetMaxTipWidth(600);
        m_tool_tips.AddTool(this, _T(""));
        SetToolTipsTopMost();       //设置提示信息总是置顶
    }

    //SetTimer(TASKBAR_TIMER, 100, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    //SaveConfig();

    //关闭所有以任务栏窗口为父窗口的窗口
    for (const auto& item : CBaseDialog::AllUniqueHandels())
    {
        HWND parent = ::GetParent(item.second);
        if (parent == GetSafeHwnd())
        {
            ::SendMessage(item.second, WM_COMMAND, IDCANCEL, 0);
        }
    }

    DestroyWindow();
    //程序关闭的时候，把最小化窗口的width恢复回去
    CheckTaskbarOnTopOrBottom();
    if (m_taskbar_on_top_or_bottom)
        ::MoveWindow(m_hMin, m_left_space, 0, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);
    else

        ::MoveWindow(m_hMin, 0, m_top_space, m_rcMinOri.Width(), m_rcMinOri.Height(), TRUE);

    //CDialogEx::OnCancel();
}


void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    m_menu_popuped = true;
    m_tool_tips.Pop();
    if (CheckClickedItem(point) && m_clicked_item.is_plugin && m_clicked_item.plugin_item != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.plugin_item);
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.plugin_item->OnMouseEvent(IPluginItem::MT_RCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }

    CPoint point1;  //定义一个用于确定光标位置的位置
    GetCursorPos(&point1);  //获取当前光标的位置，以便使得菜单可以跟随光标
    CMenu* pMenu = theApp.m_taskbar_menu.GetSubMenu(0);
    if (pMenu != nullptr)
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
    CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
    CDialogEx::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));

    pMenu->EnableMenuItem(ID_SELECT_ALL_CONNECTION, MF_BYCOMMAND | (theApp.m_general_data.show_all_interface ? MF_GRAYED : MF_ENABLED));
    pMenu->EnableMenuItem(ID_CHECK_UPDATE, MF_BYCOMMAND | (theApp.IsCheckingForUpdate() ? MF_GRAYED : MF_ENABLED));

    //pMenu->SetDefaultItem(ID_NETWORK_INFO);
    //设置默认菜单项
    switch (theApp.m_taskbar_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        pMenu->SetDefaultItem(ID_NETWORK_INFO);
        break;
    case DoubleClickAction::HISTORY_TRAFFIC:
        pMenu->SetDefaultItem(ID_TRAFFIC_HISTORY);
        break;
    case DoubleClickAction::SHOW_MORE_INFO:
        pMenu->SetDefaultItem(ID_SHOW_CPU_MEMORY2);
        break;
    case DoubleClickAction::OPTIONS:
        pMenu->SetDefaultItem(ID_OPTIONS2);
        break;
    case DoubleClickAction::TASK_MANAGER:
        pMenu->SetDefaultItem(ID_OPEN_TASK_MANAGER);
        break;
    default:
        pMenu->SetDefaultItem(-1);
        break;
    }
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_MENU_POPED_UP, 0, 0);        //通知主窗口菜单已弹出
}


BOOL CTaskBarDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    //屏蔽按回车键和ESC键退出
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) return TRUE;
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) return TRUE;

    if (theApp.m_taskbar_data.show_tool_tip && !m_menu_popuped && IsWindow(m_tool_tips.GetSafeHwnd()) && (pMsg->message == WM_LBUTTONDOWN ||
        pMsg->message == WM_LBUTTONUP ||
        pMsg->message == WM_MOUSEMOVE))
    {
        m_tool_tips.RelayEvent(pMsg);
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}


void CTaskBarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CDialogEx::OnMouseMove(nFlags, point);
}


void CTaskBarDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (CheckClickedItem(point) && m_clicked_item.is_plugin && m_clicked_item.plugin_item != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.plugin_item);
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.plugin_item->OnMouseEvent(IPluginItem::MT_DBCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }
    switch (theApp.m_taskbar_data.double_click_action)
    {
    case DoubleClickAction::CONNECTION_INFO:
        SendMessage(WM_COMMAND, ID_NETWORK_INFO);        //双击后弹出“连接详情”对话框
        break;
    case DoubleClickAction::HISTORY_TRAFFIC:
        SendMessage(WM_COMMAND, ID_TRAFFIC_HISTORY);        //双击后弹出“历史流量统计”对话框
        break;
    case DoubleClickAction::SHOW_MORE_INFO:
        PostMessage(WM_COMMAND, ID_SHOW_CPU_MEMORY2);       //切换显示CPU和内存利用率
        break;
    case DoubleClickAction::OPTIONS:
        SendMessage(WM_COMMAND, ID_OPTIONS2);       //双击后弹出“选项设置”对话框
        break;
    case DoubleClickAction::TASK_MANAGER:
        ShellExecuteW(NULL, _T("open"), (theApp.m_system_dir + L"\\Taskmgr.exe").c_str(), NULL, NULL, SW_NORMAL);       //打开任务管理器
        break;
    case DoubleClickAction::SEPCIFIC_APP:
        ShellExecuteW(NULL, _T("open"), (theApp.m_taskbar_data.double_click_exe).c_str(), NULL, NULL, SW_NORMAL);   //打开指定程序，默认任务管理器
        break;
    default:
        break;
    }
    //CDialogEx::OnLButtonDblClk(nFlags, point);
}


void CTaskBarDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //if (nIDEvent == TASKBAR_TIMER)
    //{
    //  AdjustWindowPos();
    //  //ShowInfo();
    //  Invalidate(FALSE);
    //}

    CDialogEx::OnTimer(nIDEvent);
}


BOOL CTaskBarDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    UINT uMsg = LOWORD(wParam);
    if (uMsg == ID_SELECT_ALL_CONNECTION || uMsg == ID_SELETE_CONNECTION
        || (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELETE_CONNECTION_MAX))
    {
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);    //如果点击了“选择网络连接”子菜单项，将消息转发到主窗口
        return TRUE;
    }

    //选择了“显示项目”中的插件项目
    if (uMsg >= ID_SHOW_PLUGIN_ITEM_START && uMsg <= ID_SHOW_PLUGIN_ITEM_MAX)
    {
        IPluginItem* item = theApp.m_plugins.GetItemByIndex(uMsg - ID_SHOW_PLUGIN_ITEM_START);
        if (item != nullptr)
        {
            bool displayed = theApp.m_taskbar_data.plugin_display_item.Contains(item->GetItemId());
            theApp.m_taskbar_data.plugin_display_item.SetStrContained(item->GetItemId(), !displayed);
            ::PostMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_REOPEN_TASKBAR_WND, 0, 0);
        }
    }

    return CDialogEx::OnCommand(wParam, lParam);
}


void CTaskBarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CDialogEx::OnPaint()
    ShowInfo(&dc);
}

void CTaskBarDlg::AddHisToList(DisplayItem item_type, int current_usage_percent)
{
    int& data_count{ m_history_data_count[item_type] };
    std::list<int>& list = m_map_history_data[item_type];
    //将数累加到加链表的头部，直到添加的数据数量达到TASKBAR_GRAPH_STEP的倍数时计算平均数
    if (data_count % TASKBAR_GRAPH_STEP == 0)
    {
        //计算前面累加的TASKBAR_GRAPH_STEP个数据的平均数
        if (!list.empty())
            list.front() /= TASKBAR_GRAPH_STEP;
        //将新的数据添加到末尾
        list.push_front(current_usage_percent);
    }
    else
    {
        //数累加到加链表的头部
        list.front() += current_usage_percent;
    }
    size_t graph_max_length = m_item_rects[item_type].Width();
    //判断是否超过最大长度，如果超过，将链表尾部数据移除
    if (list.size() > graph_max_length)
    {
        list.pop_back();
    }
    data_count++;
}


int CTaskBarDlg::CalculateNetspeedPercent(unsigned __int64 net_speed)
{
    int percet = 0;
    unsigned __int64 max_value{ theApp.m_taskbar_data.GetNetspeedFigureMaxValueInBytes() };

    if (net_speed >= max_value)
        percet = 100;
    else if (max_value > 0)
        percet = net_speed * 100 / max_value;
    return percet;
}

bool CTaskBarDlg::CheckClickedItem(CPoint point)
{
    for (const auto& item : m_item_rects)
    {
        if (item.second.PtInRect(point))
        {
            m_clicked_item = item.first;
            return true;
        }
    }
    m_clicked_item = TDI_UP;
    return false;
}

void CTaskBarDlg::TryDrawGraph(CDrawCommon& drawer, const CRect& value_rect, DisplayItem item_type)
{
    std::list<int>& list = m_map_history_data[item_type];
    if (theApp.m_taskbar_data.show_graph_dashed_box)
        drawer.DrawRectOutLine(value_rect, theApp.m_taskbar_data.status_bar_color, 1, true);
    int i{ -1 };
    for (const auto& item : list)
    {
        i++;
        if (i == 0)     //不绘制链表头部的数据，因为在累加中，还未取平均数
            continue;
        if (i >= value_rect.Width())
            break;
        //从右往左画线
        CPoint start_point = CPoint(value_rect.right - i, value_rect.bottom);
        int height = item * value_rect.Height() / 100;
        drawer.DrawLine(start_point, height, theApp.m_taskbar_data.status_bar_color);
    }
}


void CTaskBarDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_WND_CLOSED, 0, 0);

    CDialogEx::OnClose();
}


void CTaskBarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    if (CheckClickedItem(point) && m_clicked_item.is_plugin && m_clicked_item.plugin_item != nullptr)
    {
        ITMPlugin* plugin = theApp.m_plugins.GetPluginByItem(m_clicked_item.plugin_item);
        if (plugin != nullptr && plugin->GetAPIVersion() >= 3)
        {
            if (m_clicked_item.plugin_item->OnMouseEvent(IPluginItem::MT_LCLICKED, point.x, point.y, (void*)GetSafeHwnd(), IPluginItem::MF_TASKBAR_WND) != 0)
                return;
        }
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}


afx_msg LRESULT CTaskBarDlg::OnExitmenuloop(WPARAM wParam, LPARAM lParam)
{
    m_menu_popuped = false;
    return 0;
}
