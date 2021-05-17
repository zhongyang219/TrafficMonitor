// TaskBarDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarDlg.h"
#include "afxdialogex.h"
#include "TrafficMonitorDlg.h"


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
        iter->second.RemoveAll();
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
END_MESSAGE_MAP()


// CTaskBarDlg 消息处理程序


void CTaskBarDlg::ShowInfo(CDC* pDC)
{
    if (this->GetSafeHwnd() == NULL || pDC == nullptr || !IsWindow(this->GetSafeHwnd())) return;

    if (m_rect.IsRectEmpty() || m_rect.IsRectNull()) return;

    //设置缓冲的DC
    CDC MemDC;
    CBitmap MemBitmap;
    MemDC.CreateCompatibleDC(NULL);
    MemBitmap.CreateCompatibleBitmap(pDC, m_window_width, m_window_height);
    MemDC.SelectObject(&MemBitmap);
    //绘图
    CRect draw_rect{ m_rect };      //绘图的矩形区域
    draw_rect.MoveToXY(0, 0);
    MemDC.FillSolidRect(draw_rect, theApp.m_taskbar_data.back_color);       //填充背景色
    CDrawCommon draw;
    draw.Create(&MemDC, nullptr);
    draw.SetFont(&m_font);
    draw.SetBackColor(theApp.m_taskbar_data.back_color);

    //计算各部分的位置
    int index = 0;
    CRect item_rect{};
    int item_count = CCommon::CountOneBits(theApp.m_cfg_data.m_tbar_display_item);  //要显示的项目数量
    auto last_iter = m_item_widths.begin();
    for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
    {
        if (theApp.m_cfg_data.m_tbar_display_item & iter->first)    //如果此项需要显示出来才绘制
        {
            //任务栏在桌面顶部或底部
            if (IsTasksbarOnTopOrBottom())
            {
                if (theApp.m_taskbar_data.horizontal_arrange)   //水平排列
                {
                    if (index > 0)
                        item_rect.MoveToX(item_rect.right + theApp.DPI(4));
                    item_rect.right = item_rect.left + iter->second.TotalWidth();
                    item_rect.bottom = item_rect.top + m_window_height;
                    DrawDisplayItem(draw, iter->first, item_rect, iter->second.label_width);
                }
                else        //非水平排列时，每两个一组显示
                {
                    //在index为奇数时同时绘制两个项目
                    if (index % 2 == 1)
                    {
                        CRect item_rect_up;     //上面一个项目的矩形区域
                        if (index > 0)
                            item_rect_up.MoveToXY(item_rect.right + theApp.DPI(4), 0);
                        item_rect.left = item_rect_up.left;
                        item_rect.top = (m_window_height - TASKBAR_WND_HEIGHT / 2);
                        //确定窗口大小
                        item_rect_up.bottom = item_rect.top - 1;
                        item_rect.bottom = m_window_height;
                        int width = max(iter->second.TotalWidth(), last_iter->second.TotalWidth());
                        item_rect.right = item_rect.left + width;
                        item_rect_up.right = item_rect_up.left + width;
                        //绘制信息
                        DrawDisplayItem(draw, last_iter->first, item_rect_up, last_iter->second.label_width);
                        DrawDisplayItem(draw, iter->first, item_rect, iter->second.label_width);
                    }
                    //要绘制的项目为奇数时绘制最后一个
                    else if (item_count % 2 == 1 && index == item_count - 1)
                    {
                        item_rect.MoveToXY(item_rect.right + theApp.DPI(4), 0);
                        item_rect.bottom = TASKBAR_WND_HEIGHT / 2;
                        item_rect.right = item_rect.left + iter->second.TotalWidth();
                        DrawDisplayItem(draw, iter->first, item_rect, iter->second.label_width);
                    }
                }
            }
            //任务栏在桌面两侧
            else
            {
                if (index > 0)
                    item_rect.MoveToXY(0, item_rect.bottom + theApp.DPI(2));
                item_rect.bottom = item_rect.top + TASKBAR_WND_HEIGHT / 2;
                item_rect.right = item_rect.left + min(m_window_width, m_rcMin.Width() - theApp.DPI(2));
                DrawDisplayItem(draw, iter->first, item_rect, iter->second.label_width);
            }

            index++;
            last_iter = iter;
        }
    }

    //将缓冲区DC中的图像拷贝到屏幕中显示
    pDC->BitBlt(0, 0, m_window_width, m_window_height, &MemDC, 0, 0, SRCCOPY);
    MemBitmap.DeleteObject();
    MemDC.DeleteDC();
}

void CTaskBarDlg::DrawDisplayItem(CDrawCommon& drawer, DisplayItem type, CRect rect, int label_width)
{
    m_item_display_width[type] = rect.Width();
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
    rect_label.right = rect_label.left + label_width;
    CRect rect_value{ rect };
    rect_value.left = rect_label.right;

    // 绘制状态条
    if (type == TDI_CPU || type == TDI_MEMORY || type == TDI_GPU_USAGE || type == TDI_CPU_TEMP || type == TDI_GPU_TEMP || type == TDI_HDD_TEMP || type == TDI_MAIN_BOARD_TEMP)
    {
        if (theApp.m_taskbar_data.cm_graph_type)
        {
            switch (type)
            {
            case TDI_CPU:
                AddHisToList(type, theApp.m_cpu_usage);
                break;
            case TDI_MEMORY:
                AddHisToList(type, theApp.m_memory_usage);
                break;
            case TDI_GPU_USAGE:
                AddHisToList(type, theApp.m_gpu_usage);
                break;
            case TDI_CPU_TEMP:
                AddHisToList(type, theApp.m_cpu_temperature);
                break;
            case TDI_GPU_TEMP:
                AddHisToList(type, theApp.m_gpu_temperature);
                break;
            case TDI_HDD_TEMP:
                AddHisToList(type, theApp.m_hdd_temperature);
                break;
            case TDI_MAIN_BOARD_TEMP:
                AddHisToList(type, theApp.m_main_board_temperature);
                break;
            default:
                break;
            }
            TryDrawGraph(drawer, rect, type);
        }
        else
        {
            int value{};
            switch (type)
            {
            case TDI_CPU:
                value = theApp.m_cpu_usage;
                break;
            case TDI_MEMORY:
                value = theApp.m_memory_usage;
                break;
            case TDI_GPU_USAGE:
                value = theApp.m_gpu_usage;
                break;
            case TDI_CPU_TEMP:
                value = theApp.m_cpu_temperature;
                break;
            case TDI_GPU_TEMP:
                value = theApp.m_gpu_temperature;
                break;
            case TDI_HDD_TEMP:
                value = theApp.m_hdd_temperature;
                break;
            case TDI_MAIN_BOARD_TEMP:
                value = theApp.m_main_board_temperature;
                break;
            default:
                break;
            }
            TryDrawStatusBar(drawer, rect, value);
        }
    }

    //绘制标签
    wstring str_label = theApp.m_taskbar_data.disp_str.Get(type);
    if (theApp.m_taskbar_data.swap_up_down)
    {
        if (type == TDI_UP)
            str_label = theApp.m_taskbar_data.disp_str.Get(TDI_DOWN);
        else if (type == TDI_DOWN)
            str_label = theApp.m_taskbar_data.disp_str.Get(TDI_UP);
    }
    drawer.DrawWindowText(rect, str_label.c_str(), label_color);

    //绘制数值
    CString str_value;
    Alignment value_alignment{ theApp.m_taskbar_data.value_right_align ? Alignment::RIGHT : Alignment::LEFT };      //数值的对齐方式
    //绘制上传或下载速度
    if (type == TDI_UP || type == TDI_DOWN)
    {
        CString format_str;
        if (theApp.m_taskbar_data.hide_unit && theApp.m_taskbar_data.speed_unit != SpeedUnit::AUTO)
            format_str = _T("%s");
        else
            format_str = _T("%s/s");
        CString str_in_speed = CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_taskbar_data);
        CString str_out_speed = CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_taskbar_data);
        if (theApp.m_taskbar_data.swap_up_down)
            std::swap(str_in_speed, str_out_speed);
        if (type == TDI_UP)
        {
            str_value.Format(format_str, str_out_speed.GetString());
        }
        else
        {
            str_value.Format(format_str, str_in_speed.GetString());
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
    else if (type == TDI_CPU || type == TDI_MEMORY || type == TDI_GPU_USAGE)
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

    drawer.DrawWindowText(rect_value, str_value, text_color, value_alignment);
}

void CTaskBarDlg::TryDrawStatusBar(CDrawCommon& drawer, const CRect& rect_bar, int usage_percent)
{
    if (!theApp.m_taskbar_data.show_status_bar)
    {
        return;
    }

    CSize fill_size = CSize(rect_bar.Width() * usage_percent / 100, rect_bar.Height());
    CRect rect_fill(rect_bar.TopLeft(), fill_size);
    drawer.DrawRectOutLine(rect_bar, theApp.m_taskbar_data.status_bar_color, 1, true);
    drawer.FillRect(rect_fill, theApp.m_taskbar_data.status_bar_color);
}

bool CTaskBarDlg::AdjustWindowPos()
{
    if (this->GetSafeHwnd() == NULL || !IsWindow(this->GetSafeHwnd()))
        return false;
    CRect rcMin, rcBar;
    ::GetWindowRect(m_hMin, rcMin); //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, rcBar); //获得二级容器的区域
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
        if (rcMin.Width() != m_min_bar_width)   //如果最小化窗口的宽度改变了，重新设置任务栏窗口的位置
        {
            m_left_space = rcMin.left - rcBar.left;
            m_rcMin = rcMin;
            m_min_bar_width = m_rcMin.Width() - m_rect.Width(); //保存最小化窗口宽度
            if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            {
                ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);    //设置最小化窗口的位置
                m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
            }
            else
            {
                ::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
                m_rect.MoveToX(m_left_space);
            }
            m_rect.MoveToY((rcBar.Height() - m_rect.Height()) / 2);
            if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())
                m_rect.MoveToY(m_rect.top + theApp.DPI(1));
            MoveWindow(m_rect);
        }
    }
    else        //当任务栏在屏幕在左侧或右侧时
    {
        //设置窗口大小
        if (rcMin.Height() != m_min_bar_height) //如果最小化窗口的高度改变了，重新设置任务栏窗口的位置
        {
            m_top_space = rcMin.top - rcBar.top;
            m_rcMin = rcMin;
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
        rect.MoveToXY(rect.left + rcBar.left, rect.top + rcBar.top);
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
    if (theApp.m_taskbar_data.transparent_color != 0 && theApp.m_taksbar_transparent_color_enable)
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
    temp.Format(_T("%s: %s (%s: %s/%s: %s)"), CCommon::LoadText(IDS_TRAFFIC_USED_TODAY),
        CCommon::KBytesToString((theApp.m_today_up_traffic + theApp.m_today_down_traffic) / 1024u),
        CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(theApp.m_today_up_traffic / 1024u),
        CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(theApp.m_today_down_traffic / 1024u)
    );
    tip_info += temp;
    if (!IsShowUp())
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_UPLOAD),
            CCommon::DataSizeToString(theApp.m_out_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!IsShowDown())
    {
        temp.Format(_T("\r\n%s: %s/s"), CCommon::LoadText(IDS_DOWNLOAD),
            CCommon::DataSizeToString(theApp.m_in_speed, theApp.m_main_wnd_data));
        tip_info += temp;
    }
    if (!IsShowCpu())
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
    if (!IsShowMemory())
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
        if (!IsShowGpu() && theApp.m_gpu_usage >= 0)
        {
            temp.Format(_T("\r\n%s: %d %%"), CCommon::LoadText(IDS_GPU_USAGE), theApp.m_gpu_usage);
            tip_info += temp;
        }
        if (!IsShowCpuTemperature() && theApp.m_cpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_CPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_cpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsShowGpuTemperature() && theApp.m_gpu_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_GPU_TEMPERATURE), CCommon::TemperatureToString(theApp.m_gpu_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsShowHddTemperature() && theApp.m_hdd_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_HDD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_hdd_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
        if (!IsShowMainboardTemperature() && theApp.m_main_board_temperature > 0)
        {
            temp.Format(_T("\r\n%s: %s"), CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE), CCommon::TemperatureToString(theApp.m_main_board_temperature, theApp.m_taskbar_data));
            tip_info += temp;
        }
    }
#endif
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
    if (theApp.m_cfg_data.m_tbar_display_item == 0)
        theApp.m_cfg_data.m_tbar_display_item |= TDI_UP;        //至少显示一项
    int item_count = CCommon::CountOneBits(theApp.m_cfg_data.m_tbar_display_item);

    m_item_widths.clear();

    m_pDC->SelectObject(&m_font);
    //计算标签宽度
    const auto& item_map = theApp.m_taskbar_data.disp_str.GetAllItems();
    for (auto iter = item_map.begin(); iter != item_map.end(); ++iter)
    {
        m_item_widths[iter->first].label_width = m_pDC->GetTextExtent(iter->second.c_str()).cx;
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
    m_item_widths[TDI_UP].value_width = value_width;
    m_item_widths[TDI_DOWN].value_width = value_width;

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
    m_item_widths[TDI_CPU].value_width = value_width;
    m_item_widths[TDI_MEMORY].value_width = memory_width;
    m_item_widths[TDI_GPU_USAGE].value_width = value_width;

    //计算温度显示的宽度
    if (theApp.m_taskbar_data.separate_value_unit_with_space)
        str = _T("99 °C");
    else
        str = _T("99°C");
    value_width = m_pDC->GetTextExtent(str).cx;
    value_width += theApp.DPI(2);
    m_item_widths[TDI_CPU_TEMP].value_width = value_width;
    m_item_widths[TDI_GPU_TEMP].value_width = value_width;
    m_item_widths[TDI_HDD_TEMP].value_width = value_width;
    m_item_widths[TDI_MAIN_BOARD_TEMP].value_width = value_width;

    //计算窗口总宽度
    if (IsTasksbarOnTopOrBottom())  //任务栏在桌面的顶部或底部时
    {
        m_window_width = 0;
        if (theApp.m_taskbar_data.horizontal_arrange)   //水平排列时
        {
            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                if (theApp.m_cfg_data.m_tbar_display_item & iter->first)
                    m_window_width += iter->second.TotalWidth();
            }
            m_window_width += theApp.DPI(4) * item_count;   //加上每个标签间的空隙
        }
        else        //非水平排列时，每两个一组排列
        {
            int index = 0;
            int width0;
            for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
            {
                if (theApp.m_cfg_data.m_tbar_display_item & iter->first)
                {
                    if (index % 2 == 0)
                    {
                        width0 = iter->second.TotalWidth();
                    }
                    else
                    {
                        m_window_width += max(width0, iter->second.TotalWidth());
                    }
                    if (item_count % 2 == 1 && index == item_count - 1) //项目数为奇数时加上最后一个的宽度
                    {
                        m_window_width += iter->second.TotalWidth();
                    }

                    index++;
                }
            }
            m_window_width += theApp.DPI(4) * ((item_count + 1) / 2 + 1);   //加上每个标签间的空隙
        }
    }
    else        //任务栏在桌面两侧时
    {
        m_window_width = 0;
        //所有标签中最大的宽度即为窗口宽度
        for (auto iter = m_item_widths.begin(); iter != m_item_widths.end(); ++iter)
        {
            if (theApp.m_cfg_data.m_tbar_display_item & iter->first)
            {
                if (m_window_width < iter->second.TotalWidth())
                    m_window_width = iter->second.TotalWidth();
            }
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
        m_window_height += (theApp.DPI(2) * item_count);   //加上每个标签间的空隙
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

bool CTaskBarDlg::IsShowCpuMemory()
{
    return ((theApp.m_cfg_data.m_tbar_display_item & TDI_CPU) || (theApp.m_cfg_data.m_tbar_display_item & TDI_MEMORY));
}

bool CTaskBarDlg::IsShowNetSpeed()
{
    return ((theApp.m_cfg_data.m_tbar_display_item & TDI_UP) || (theApp.m_cfg_data.m_tbar_display_item & TDI_DOWN));
}

bool CTaskBarDlg::IsShowUp()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_UP);
}

bool CTaskBarDlg::IsShowDown()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_DOWN);
}

bool CTaskBarDlg::IsShowCpu()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_CPU);
}

bool CTaskBarDlg::IsShowMemory()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_MEMORY);
}

bool CTaskBarDlg::IsShowGpu()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_GPU_USAGE);
}

bool CTaskBarDlg::IsShowCpuTemperature()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_CPU_TEMP);
}

bool CTaskBarDlg::IsShowGpuTemperature()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_GPU_TEMP);
}

bool CTaskBarDlg::IsShowHddTemperature()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_HDD_TEMP);
}

bool CTaskBarDlg::IsShowMainboardTemperature()
{
    return (theApp.m_cfg_data.m_tbar_display_item & TDI_MAIN_BOARD_TEMP);
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

    //设置窗口透明色
    ApplyWindowTransparentColor();

    ::GetWindowRect(m_hMin, m_rcMin);   //获得最小化窗口的区域
    ::GetWindowRect(m_hBar, m_rcBar);   //获得二级容器的区域
    m_left_space = m_rcMin.left - m_rcBar.left;
    m_top_space = m_rcMin.top - m_rcBar.top;

    CheckTaskbarOnTopOrBottom();
    CalculateWindowSize();
    m_rect.SetRectEmpty();
    m_rect.bottom = m_window_height;
    m_rect.right = m_rect.left + m_window_width;

    if (m_taskbar_on_top_or_bottom)     //如果任务栏在桌面顶部或底部
    {
        m_min_bar_width = m_rcMin.Width() - m_rect.Width(); //保存最小化窗口宽度

        //通过用MoveWindow函数来改变小化窗口的宽度
        if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);
        else
            ::MoveWindow(m_hMin, m_left_space + m_rect.Width(), 0, m_rcMin.Width() - m_rect.Width(), m_rcMin.Height(), TRUE);

        m_connot_insert_to_task_bar = !(::SetParent(this->m_hWnd, m_hBar)); //把程序窗口设置成任务栏的子窗口
        //m_connot_insert_to_task_bar = true;
        m_error_code = GetLastError();

        //调整程序窗口的大小和位置
        if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            m_rect.MoveToX(m_left_space + m_rcMin.Width() - m_rect.Width() + 2);
        else
            m_rect.MoveToX(m_left_space);
        m_rect.MoveToY((m_rcBar.Height() - m_rect.Height()) / 2);
        if (theApp.m_taskbar_data.horizontal_arrange && theApp.m_win_version.IsWindows7())      //系统是Win7并且水平排列时，任务栏窗口位置向下调整一个像素
            m_rect.MoveToY(m_rect.top + theApp.DPI(1));
        //::MoveWindow(this->m_hWnd, wnd_x_pos, wnd_y_pos, m_rect.Width(), m_rect.Height(), TRUE);
        MoveWindow(m_rect);
    }
    else    //当任务栏在桌面左侧或右侧时
    {
        //设置窗口大小
        m_min_bar_height = m_rcMin.Height() - m_rect.Height();  //保存最小化窗口高度

        //通过用MoveWindow函数来改变小化窗口的高度
        if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            ::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);
        else
            ::MoveWindow(m_hMin, 0, m_top_space + m_rect.Height(), m_rcMin.Width(), m_rcMin.Height() - m_rect.Height(), TRUE);

        m_connot_insert_to_task_bar = (!::SetParent(this->m_hWnd, m_hBar)); //把程序窗口设置成任务栏的子窗口
        //m_connot_insert_to_task_bar = true;
        m_error_code = GetLastError();

        //调整程序窗口的大小和位置
        m_rect.MoveToX((m_rcMin.Width() - m_window_width) / 2);
        if (m_rect.left < theApp.DPI(2))
            m_rect.MoveToX(theApp.DPI(2));
        if (!theApp.m_taskbar_data.tbar_wnd_on_left)
            m_rect.MoveToY(m_top_space + m_rcMin.Height() - m_rect.Height() + 2);
        else
            m_rect.MoveToY(m_top_space);
        MoveWindow(m_rect);
    }
    CRect rect{ m_rect };
    //如果窗口没有被成功嵌入到任务栏，窗口移动到了基于屏幕左上角的绝对位置，则修正窗口的位置
    if (m_connot_insert_to_task_bar)
    {
        rect.MoveToXY(rect.left + m_rcBar.left, rect.top + m_rcBar.top);
        this->MoveWindow(rect);
    }

    SetBackgroundColor(theApp.m_taskbar_data.back_color);

    //初始化鼠标提示
    m_tool_tips.Create(this, TTS_ALWAYSTIP);
    m_tool_tips.SetMaxTipWidth(600);
    m_tool_tips.AddTool(this, _T(""));
    SetToolTipsTopMost();       //设置提示信息总是置顶

    //SetTimer(TASKBAR_TIMER, 100, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarDlg::OnCancel()
{
    // TODO: 在此添加专用代码和/或调用基类
    //SaveConfig();
    DestroyWindow();
    //程序关闭的时候，把最小化窗口的width恢复回去
    CheckTaskbarOnTopOrBottom();
    if (m_taskbar_on_top_or_bottom)
        ::MoveWindow(m_hMin, m_left_space, 0, m_rcMin.Width(), m_rcMin.Height(), TRUE);
    else

        ::MoveWindow(m_hMin, 0, m_top_space, m_rcMin.Width(), m_rcMin.Height(), TRUE);

    //CDialogEx::OnCancel();
}


void CTaskBarDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值

    CPoint point1;  //定义一个用于确定光标位置的位置
    GetCursorPos(&point1);  //获取当前光标的位置，以便使得菜单可以跟随光标
    theApp.m_taskbar_menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单
    CDialogEx::OnRButtonUp(nFlags, point1);
}


void CTaskBarDlg::OnInitMenu(CMenu* pMenu)
{
    CDialogEx::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    pMenu->CheckMenuItem(ID_SHOW_CPU_MEMORY2, MF_BYCOMMAND | (IsShowCpuMemory() ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_NET_SPEED, MF_BYCOMMAND | ((IsShowNetSpeed() || !IsShowMemory()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MAIN_WND, MF_BYCOMMAND | (!theApp.m_cfg_data.m_hide_main_window ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_NOTIFY_ICON, MF_BYCOMMAND | (theApp.m_cfg_data.m_show_notify_icon ? MF_CHECKED : MF_UNCHECKED));

    pMenu->CheckMenuItem(ID_SHOW_UP_SPEED, MF_BYCOMMAND | ((IsShowUp()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_DOWN_SPEED, MF_BYCOMMAND | ((IsShowDown()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_CPU_USAGE, MF_BYCOMMAND | ((IsShowCpu()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MEMORY_USAGE, MF_BYCOMMAND | ((IsShowMemory()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_GPU, MF_BYCOMMAND | ((IsShowGpu()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_CPU_TEMPERATURE, MF_BYCOMMAND | ((IsShowCpuTemperature()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_GPU_TEMPERATURE, MF_BYCOMMAND | ((IsShowGpuTemperature()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_HDD_TEMPERATURE, MF_BYCOMMAND | ((IsShowHddTemperature()) ? MF_CHECKED : MF_UNCHECKED));
    pMenu->CheckMenuItem(ID_SHOW_MAIN_BOARD_TEMPERATURE, MF_BYCOMMAND | ((IsShowMainboardTemperature()) ? MF_CHECKED : MF_UNCHECKED));

    //不含温度监控的版本，禁用温度监控相关菜单项
#ifdef WITHOUT_TEMPERATURE
    pMenu->EnableMenuItem(ID_SHOW_GPU, MF_BYCOMMAND | MF_GRAYED);
    pMenu->EnableMenuItem(ID_SHOW_CPU_TEMPERATURE, MF_BYCOMMAND | MF_GRAYED);
    pMenu->EnableMenuItem(ID_SHOW_GPU_TEMPERATURE, MF_BYCOMMAND | MF_GRAYED);
    pMenu->EnableMenuItem(ID_SHOW_HDD_TEMPERATURE, MF_BYCOMMAND | MF_GRAYED);
    pMenu->EnableMenuItem(ID_SHOW_MAIN_BOARD_TEMPERATURE, MF_BYCOMMAND | MF_GRAYED);
#endif

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

    if (theApp.m_taskbar_data.show_tool_tip && m_tool_tips.GetSafeHwnd() && (pMsg->message == WM_LBUTTONDOWN ||
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
        || (uMsg > ID_SELECT_ALL_CONNECTION && uMsg <= ID_SELECT_ALL_CONNECTION + 98))
    {
        ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_COMMAND, wParam, lParam);    //如果点击了“选择网络连接”子菜单项，将消息转发到主窗口
        return TRUE;
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
    CList<int, int>& list = m_map_history_data[item_type];
    list.AddHead(current_usage_percent);
    int graph_max_length = m_item_display_width[item_type] * TASKBAR_GRAPH_STEP;
    //判断是否超过最大长度，如果超过，将链表尾部数据移除
    if (list.GetCount() > graph_max_length)
    {
        list.RemoveTail();
    }
}


void CTaskBarDlg::TryDrawGraph(CDrawCommon& drawer, const CRect& value_rect, DisplayItem item_type)
{
    if (!theApp.m_taskbar_data.show_status_bar)
    {
        return;
    }
    CList<int, int>& list = m_map_history_data[item_type];
    drawer.DrawRectOutLine(value_rect, theApp.m_taskbar_data.status_bar_color, 1, true);
    POSITION pos = list.GetHeadPosition();
    if (NULL != pos)
    {
        //有数据才需要画线
        for (int i = 0; i < value_rect.Width(); i++)
        {
            //从右往左画线

            CPoint start_point = CPoint(value_rect.right - i, value_rect.bottom);
            int height = 0;

            for (int j = 0; j < TASKBAR_GRAPH_STEP; j++)
            {
                height = list.GetNext(pos) * value_rect.Height() / 100;
                if (NULL == pos)
                {
                    //没数据了直接返回。
                    return;
                }

            }
            drawer.DrawLine(start_point, height, theApp.m_taskbar_data.status_bar_color);
        }
    }
}


void CTaskBarDlg::OnClose()
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    ::SendMessage(theApp.m_pMainWnd->GetSafeHwnd(), WM_TASKBAR_WND_CLOSED, 0, 0);

    CDialogEx::OnClose();
}
