// HistoryTrafficCalendarDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "HistoryTrafficCalendarDlg.h"
#include "afxdialogex.h"


// CHistoryTrafficCalendarDlg 对话框

IMPLEMENT_DYNAMIC(CHistoryTrafficCalendarDlg, CTabDlg)

CHistoryTrafficCalendarDlg::CHistoryTrafficCalendarDlg(deque<HistoryTraffic>& history_traffics, CWnd* pParent /*=nullptr*/)
    : CTabDlg(IDD_HISTORY_TRAFFIC_CALENDAR_DIALOG, pParent), m_history_traffics(history_traffics)
{

}

CHistoryTrafficCalendarDlg::~CHistoryTrafficCalendarDlg()
{
}

void CHistoryTrafficCalendarDlg::SetDayTraffic()
{
    for (int i{}; i < CALENDAR_HEIGHT; i++)
    {
        for (int j{}; j < CALENDAR_WIDTH; j++)
        {
            HistoryTraffic history_traffic;
            history_traffic.year = m_year;
            history_traffic.month = m_month;
            history_traffic.day = m_calendar[i][j].day;
            //使用二分法查找日历中每一个日期的流量
            if (history_traffic.day > 0)
            {
                if (std::binary_search(m_history_traffics.begin(), m_history_traffics.end(), history_traffic, HistoryTraffic::DateGreater))
                {
                    auto iter = std::lower_bound(m_history_traffics.begin(), m_history_traffics.end(), history_traffic, HistoryTraffic::DateGreater);
                    if (iter != m_history_traffics.end())
                    {
                        m_calendar[i][j].up_traffic = iter->up_kBytes;
                        m_calendar[i][j].down_traffic = iter->down_kBytes;
                        m_calendar[i][j].mixed = iter->mixed;

                    }
                }
            }
        }
    }
}

void CHistoryTrafficCalendarDlg::MonthSelectChanged()
{
    CCalendarHelper::GetCalendar(m_year, m_month, m_calendar, theApp.m_cfg_data.m_sunday_first);
    SetDayTraffic();
    CalculateMonthTotalTraffic();
    InvalidateRect(m_draw_rect);
}

void CHistoryTrafficCalendarDlg::CalculateMonthTotalTraffic()
{
    m_month_total_upload = 0;
    m_month_total_download = 0;
    for (int i{}; i < CALENDAR_HEIGHT; i++)
    {
        for (int j{}; j < CALENDAR_WIDTH; j++)
        {
            m_month_total_upload += m_calendar[i][j].up_traffic;
            m_month_total_download += m_calendar[i][j].down_traffic;
        }
    }
}

void CHistoryTrafficCalendarDlg::SetComboSel()
{
    int cnt{};
    int year_selected;
    for (int i{ m_year_max }; i >= m_year_min; i--)
    {
        if (i == m_year)
            year_selected = cnt;
        cnt++;
    }
    m_year_combo.SetCurSel(year_selected);
    m_month_combo.SetCurSel(m_month - 1);
}

void CHistoryTrafficCalendarDlg::DoDataExchange(CDataExchange* pDX)
{
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_YEAR_COMBO, m_year_combo);
    DDX_Control(pDX, IDC_MONTH_COMBO, m_month_combo);
}


bool CHistoryTrafficCalendarDlg::IsWeekend(int index)
{
    if (theApp.m_cfg_data.m_sunday_first)
        return (index == 0 || index == 6);
    else
        return (index == 5 || index == 6);
}

CString CHistoryTrafficCalendarDlg::GetWeekdayString(int index)
{
    if (!theApp.m_cfg_data.m_sunday_first)
    {
        index++;
        if (index > 6)
            index = 0;
    }
    switch (index)
    {
    case 0:
        return CCommon::LoadText(IDS_SUNDAY);
    case 1:
        return CCommon::LoadText(IDS_MONDAY);
    case 2:
        return CCommon::LoadText(IDS_TUESDAY);
    case 3:
        return CCommon::LoadText(IDS_WEDNESDAY);
    case 4:
        return CCommon::LoadText(IDS_THURSDAY);
    case 5:
        return CCommon::LoadText(IDS_FRIDAY);
    case 6:
        return CCommon::LoadText(IDS_SATURDAY);
    }
    return CString();
}

BEGIN_MESSAGE_MAP(CHistoryTrafficCalendarDlg, CTabDlg)
    ON_WM_PAINT()
    ON_CBN_SELCHANGE(IDC_YEAR_COMBO, &CHistoryTrafficCalendarDlg::OnCbnSelchangeYearCombo)
    ON_CBN_SELCHANGE(IDC_MONTH_COMBO, &CHistoryTrafficCalendarDlg::OnCbnSelchangeMonthCombo)
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CHistoryTrafficCalendarDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CHistoryTrafficCalendarDlg::OnBnClickedNextButton)
    ON_WM_MOUSEWHEEL()
    ON_BN_CLICKED(IDC_MENU_BUTTON, &CHistoryTrafficCalendarDlg::OnBnClickedMenuButton)
    ON_WM_INITMENU()
    ON_COMMAND(ID_FIRST_DAY_OF_WEEK_SUNDAY, &CHistoryTrafficCalendarDlg::OnFirstDayOfWeekSunday)
    ON_COMMAND(ID_FIRST_DAY_OF_WEEK_MONDAY, &CHistoryTrafficCalendarDlg::OnFirstDayOfWeekMonday)
    ON_COMMAND(ID_CALENDAR_JUMP_TO_TODAY, &CHistoryTrafficCalendarDlg::OnCalendarJumpToToday)
END_MESSAGE_MAP()


// CHistoryTrafficCalendarDlg 消息处理程序


BOOL CHistoryTrafficCalendarDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化
    m_year = m_history_traffics[0].year;
    m_month = m_history_traffics[0].month;
    CCalendarHelper::GetCalendar(m_year, m_month, m_calendar, theApp.m_cfg_data.m_sunday_first);
    SetDayTraffic();
    CalculateMonthTotalTraffic();

    //初始化Combo Box
    m_year_max = m_history_traffics[0].year;
    m_year_min = m_history_traffics.back().year;
    for (int i{ m_year_max }; i >= m_year_min; i--)
    {
        m_year_combo.AddString(CCommon::IntToString(i));
    }
    m_year_combo.SetCurSel(0);
    for (int i{ 1 }; i <= 12; i++)
    {
        m_month_combo.AddString(CCommon::IntToString(i));
    }
    m_month_combo.SetCurSel(m_month - 1);

    //初始化鼠标提示
    m_tool_tips.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
    m_tool_tips.SetMaxTipWidth(800);        //为鼠标提示设置一个最大宽度，以允许其换行
    m_tool_tips.AddTool(this, _T(""));

    CCommon::LoadMenuResource(m_menu, IDR_HISTORY_TRAFFIC_MENU);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CHistoryTrafficCalendarDlg::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: 在此处添加消息处理程序代码
                       // 不为绘图消息调用 CTabDlg::OnPaint()
    CRect wndRect;
    GetWindowRect(wndRect);
    const int width = theApp.DPI(40);       //日历中每一个格子的宽度
    const int height = theApp.DPI(30);      //日历中第一个格子的高度
    m_draw_rect.left = m_start_x;
    m_draw_rect.top = m_start_y;
    //m_draw_rect.right = m_draw_rect.left + (CALENDAR_WIDTH*width);
    m_draw_rect.right = wndRect.Width() - m_start_x;
    m_draw_rect.bottom = m_draw_rect.top + ((CALENDAR_HEIGHT + 2) * height) + theApp.DPI(20);

    //使用双缓冲绘图
    CDrawDoubleBuffer draw_double_buffer(&dc, m_draw_rect);

    //绘图
    CDrawCommon draw;
    draw.Create(draw_double_buffer.GetMemDC(), this);
    draw.FillRect(CRect(0, 0, m_draw_rect.Width(), m_draw_rect.Height()), RGB(255, 255, 255));      //填充白色背景色
    CRect rect{};
    rect.left = 0;
    rect.top = 0;
    rect.right = rect.left + width;
    rect.bottom = rect.top + height;
    //画星期的行
    for (int i{}; i < CALENDAR_WIDTH; i++)
    {
        rect.MoveToX(i * width);

        if (IsWeekend(i))
            draw.SetBackColor(RGB(217, 86, 86));
        else if (i % 2 == 0)
            draw.SetBackColor(RGB(1, 84, 151));
        else
            draw.SetBackColor(RGB(1, 107, 191));
        CString str = GetWeekdayString(i);
        draw.DrawWindowText(rect, str, RGB(255, 255, 255), Alignment::CENTER, true);
    }

    //绘制日历
    for (int i{}; i < CALENDAR_HEIGHT; i++)
    {
        for (int j{}; j < CALENDAR_WIDTH; j++)
        {
            //设置日历中每一天的矩形的位置
            rect.MoveToXY(j * width, height + theApp.DPI(2) + i * height);
            //保存矩形的位置
            m_calendar[i][j].rect = rect;
            //绘制日期的数据
            //绘制格子的背景颜色
            if (IsWeekend(j))       //是周末时
            {
                if ((i + j) % 2 == 0)
                    draw.SetBackColor(RGB(250, 234, 234));
                else
                    draw.SetBackColor(RGB(252, 242, 242));
            }
            else
            {
                if ((i + j) % 2 == 0)
                    draw.SetBackColor(RGB(226, 241, 254));
                else
                    draw.SetBackColor(RGB(236, 246, 254));
            }
            draw.FillRectWithBackColor(rect);
            //绘制格子上的日期的数字
            CRect day_rect{ rect };
            day_rect.bottom -= (rect.Height() / 2);
            COLORREF text_color;
            if (IsWeekend(j))
                text_color = RGB(131, 29, 28);
            else
                text_color = RGB(0, 57, 107);
            if (m_calendar[i][j].day != 0)
                draw.DrawWindowText(day_rect, CCommon::IntToString(m_calendar[i][j].day), text_color, Alignment::CENTER, true);
            //在今天的日期上画一个矩形框
            COLORREF frame_color;
            if (IsWeekend(j))
                frame_color = RGB(218, 91, 91);
            else
                frame_color = RGB(1, 133, 238);
            if (m_year == m_history_traffics[0].year && m_month == m_history_traffics[0].month && m_calendar[i][j].day == m_history_traffics[0].day)
                draw.DrawRectOutLine(rect, frame_color, theApp.DPI(2));

            //绘制指示流量大小的矩形
            COLORREF color;
            if (m_calendar[i][j].traffic() < 1024 * 1024)       //流量小于1GB时绘制蓝色
                color = TRAFFIC_COLOR_BLUE;
            else if (m_calendar[i][j].traffic() < 10 * 1024 * 1024) //流量小于10GB时绘制绿色
                color = TRAFFIC_COLOR_GREEN;
            else if (m_calendar[i][j].traffic() < 100 * 1024 * 1024)    //流量小于100GB时绘制黄色
                color = TRAFFIC_COLOR_YELLOE;
            else if (m_calendar[i][j].traffic() < 1024 * 1024 * 1024)   //流量小于1TB时绘制红色
                color = TRAFFIC_COLOR_RED;
            else        //流量超过1TB时显示深红色
                color = TRAFFIC_COLOR_DARK_RED;
            if (m_calendar[i][j].traffic() > 0)
            {
                CRect traffic_rect;
                traffic_rect.left = rect.left + theApp.DPI(14);
                traffic_rect.right = traffic_rect.left + theApp.DPI(12);
                traffic_rect.top = rect.top + theApp.DPI(16);
                traffic_rect.bottom = traffic_rect.top + theApp.DPI(12);
                draw.FillRect(traffic_rect, color);
            }
        }
    }

    const COLORREF text_color{ RGB(0, 57, 107) };
    //画当前月总流量
    CString info;
    info.Format(_T("%s %s (%s: %s, %s: %s)"), CCommon::LoadText(IDS_CURRENT_MONTH_TOTAL_TRAFFIC),
        CCommon::KBytesToString(m_month_total_upload + m_month_total_download),
        CCommon::LoadText(IDS_UPLOAD), CCommon::KBytesToString(m_month_total_upload),
        CCommon::LoadText(IDS_DOWNLOAD), CCommon::KBytesToString(m_month_total_download)
    );
    CRect info_rect;
    info_rect.left = 0;
    info_rect.top = height * (CALENDAR_HEIGHT + 1) + theApp.DPI(5);
    info_rect.right = info_rect.left + m_draw_rect.Width();
    info_rect.bottom = info_rect.top + theApp.DPI(36);
    draw.SetBackColor(RGB(255, 255, 255));
    draw.DrawWindowText(info_rect, info, text_color, Alignment::LEFT, true, true);

    //画图例
    CRect rc_legend{ info_rect };
    rc_legend.MoveToXY(CALENDAR_WIDTH * width + theApp.DPI(32), theApp.DPI(16));
    rc_legend.right = m_draw_rect.Width();
    rc_legend.bottom = rc_legend.top + theApp.DPI(16);

    draw.DrawWindowText(rc_legend, CCommon::LoadText(IDS_LEGEND, _T(":")), text_color, Alignment::LEFT, true, true);
    rc_legend.MoveToY(rc_legend.bottom + theApp.DPI(6));
    CRect rc_legend_box{ rc_legend };
    const int box_side{ theApp.DPI(12) };
    const int line_gap{ theApp.DPI(6) };
    rc_legend_box.top = rc_legend.top + (rc_legend.Height() - box_side) / 2;
    rc_legend_box.bottom = rc_legend_box.top + box_side;
    rc_legend_box.right = rc_legend_box.left + box_side;
    rc_legend.left = rc_legend_box.right + theApp.DPI(4);
    //蓝色图例
    draw.FillRect(rc_legend_box, TRAFFIC_COLOR_BLUE);
    draw.DrawWindowText(rc_legend, _T("0~1GB"), text_color);
    //绿色图例
    rc_legend.MoveToY(rc_legend.bottom + line_gap);
    rc_legend_box.MoveToY(rc_legend.top + (rc_legend.Height() - box_side) / 2);
    draw.FillRect(rc_legend_box, TRAFFIC_COLOR_GREEN);
    draw.DrawWindowText(rc_legend, _T("1GB~10GB"), text_color);
    //黄色图例
    rc_legend.MoveToY(rc_legend.bottom + line_gap);
    rc_legend_box.MoveToY(rc_legend.top + (rc_legend.Height() - box_side) / 2);
    draw.FillRect(rc_legend_box, TRAFFIC_COLOR_YELLOE);
    draw.DrawWindowText(rc_legend, _T("10GB~100GB"), text_color);
    //红色图例
    rc_legend.MoveToY(rc_legend.bottom + line_gap);
    rc_legend_box.MoveToY(rc_legend.top + (rc_legend.Height() - box_side) / 2);
    draw.FillRect(rc_legend_box, TRAFFIC_COLOR_RED);
    draw.DrawWindowText(rc_legend, _T("100GB~1TB"), text_color);
    //深红色图例
    rc_legend.MoveToY(rc_legend.bottom + line_gap);
    rc_legend_box.MoveToY(rc_legend.top + (rc_legend.Height() - box_side) / 2);
    draw.FillRect(rc_legend_box, TRAFFIC_COLOR_DARK_RED);
    draw.DrawWindowText(rc_legend, _T("1TB~"), text_color);
}


void CHistoryTrafficCalendarDlg::OnCbnSelchangeYearCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    int index = m_year_combo.GetCurSel();
    CString str;
    m_year_combo.GetLBText(index, str);
    m_year = _ttoi(str);
    MonthSelectChanged();
}


void CHistoryTrafficCalendarDlg::OnCbnSelchangeMonthCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_month = m_month_combo.GetCurSel() + 1;
    MonthSelectChanged();
}


void CHistoryTrafficCalendarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    int tip_day{};      //要显示鼠标提示的日期
    DayTraffic tip_traffic{};
    static int last_tip_day{};
    //查找鼠标指针在哪个日期的矩形内
    for (int i{}; i < CALENDAR_HEIGHT; i++)
    {
        for (int j{}; j < CALENDAR_WIDTH; j++)
        {
            CRect rect{ m_calendar[i][j].rect };
            rect.OffsetRect(m_start_x, m_start_y);
            if (rect.PtInRect(point))
            {
                tip_day = m_calendar[i][j].day;
                tip_traffic = m_calendar[i][j];
            }
        }
    }
    bool show_tip = (tip_day > 0);
    if (show_tip && last_tip_day != tip_day)
    {
        CString tip_info;
        tip_info.Format(_T("%d/%d/%d\r\n"), m_year, m_month, tip_day);
        tip_info += CCommon::LoadText(IDS_TRAFFIC_USED1);
        tip_info += CCommon::KBytesToString(tip_traffic.traffic());
        if (!tip_traffic.mixed && tip_traffic.traffic() > 0)
        {
            tip_info += _T("\r\n");
            tip_info += CCommon::LoadText(IDS_UPLOAD, _T(": "));
            tip_info += CCommon::KBytesToString(tip_traffic.up_traffic);
            tip_info += _T("\r\n");
            tip_info += CCommon::LoadText(IDS_DOWNLOAD, _T(": "));
            tip_info += CCommon::KBytesToString(tip_traffic.down_traffic);
        }

        m_tool_tips.AddTool(this, tip_info);
        m_tool_tips.Pop();
        last_tip_day = tip_day;
    }
    if (!show_tip)
    {
        m_tool_tips.AddTool(this, _T(""));
        m_tool_tips.Pop();
        last_tip_day = 0;
    }

    CTabDlg::OnMouseMove(nFlags, point);
}


BOOL CHistoryTrafficCalendarDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (m_tool_tips.GetSafeHwnd() != 0)
    {
        m_tool_tips.RelayEvent(pMsg);
    }

    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_LEFT)
        {
            OnBnClickedPreviousButton();
            return TRUE;
        }
        if (pMsg->wParam == VK_RIGHT)
        {
            OnBnClickedNextButton();
            return TRUE;
        }
    }

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CHistoryTrafficCalendarDlg::OnBnClickedPreviousButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_year == m_year_min && m_month == 1)
        return;
    m_month--;
    if (m_month <= 0)
    {
        m_month = 12;
        m_year--;
    }
    SetComboSel();
    MonthSelectChanged();
}


void CHistoryTrafficCalendarDlg::OnBnClickedNextButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_year == m_year_max && m_month == 12)
        return;
    m_month++;
    if (m_month > 12)
    {
        m_month = 1;
        m_year++;
    }
    SetComboSel();
    MonthSelectChanged();
}


BOOL CHistoryTrafficCalendarDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //通过鼠标滚轮翻页
    if (zDelta > 0)
    {
        OnBnClickedPreviousButton();
    }
    if (zDelta < 0)
    {
        OnBnClickedNextButton();
    }

    return CTabDlg::OnMouseWheel(nFlags, zDelta, pt);
}


void CHistoryTrafficCalendarDlg::OnBnClickedMenuButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CWnd* pBtn = GetDlgItem(IDC_MENU_BUTTON);
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        CPoint point;
        point.x = rect.left;
        point.y = rect.bottom;
        CMenu* pMenu = m_menu.GetSubMenu(1);
        if (pMenu != NULL)
            pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CHistoryTrafficCalendarDlg::OnInitMenu(CMenu* pMenu)
{
    CTabDlg::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    if (theApp.m_cfg_data.m_sunday_first)
        pMenu->CheckMenuRadioItem(ID_FIRST_DAY_OF_WEEK_SUNDAY, ID_FIRST_DAY_OF_WEEK_MONDAY, ID_FIRST_DAY_OF_WEEK_SUNDAY, MF_BYCOMMAND | MF_CHECKED);
    else
        pMenu->CheckMenuRadioItem(ID_FIRST_DAY_OF_WEEK_SUNDAY, ID_FIRST_DAY_OF_WEEK_MONDAY, ID_FIRST_DAY_OF_WEEK_MONDAY, MF_BYCOMMAND | MF_CHECKED);

}


void CHistoryTrafficCalendarDlg::OnFirstDayOfWeekSunday()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_cfg_data.m_sunday_first = true;
    MonthSelectChanged();
}


void CHistoryTrafficCalendarDlg::OnFirstDayOfWeekMonday()
{
    // TODO: 在此添加命令处理程序代码
    theApp.m_cfg_data.m_sunday_first = false;
    MonthSelectChanged();
}


void CHistoryTrafficCalendarDlg::OnCalendarJumpToToday()
{
    // TODO: 在此添加命令处理程序代码
    m_year = m_history_traffics[0].year;
    m_month = m_history_traffics[0].month;
    SetComboSel();
    MonthSelectChanged();

}
