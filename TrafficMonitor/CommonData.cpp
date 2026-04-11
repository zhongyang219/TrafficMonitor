#include "stdafx.h"
#include "CommonData.h"
#include "Common.h"
#include "CalendarHelper.h"
#include "TrafficMonitor.h"
#include "WindowsSettingHelper.h"

///////////////////////////////////////////////////////////////////////////////////
int Date::week() const
{
    //计算当前是一年的第几天
    int days{};
    for (int i{ 1 }; i < month; i++)
    {
        days += CCalendarHelper::DaysInMonth(year, i);
    }
    days += day;
    //计算这一年的1月1日是星期几
    int week_day = CCalendarHelper::CaculateWeekDay(year, 1, 1);
    if (theApp.m_cfg_data.m_sunday_first)
    {
        days += (week_day - 1);
    }
    else
    {
        days += (week_day - 2);
    }
    return days / 7 + 1;
}

bool Date::DateGreater(const Date& a, const Date& b)
{
    if (a.year != b.year)
        return a.year > b.year;
    else if (a.month != b.month)
        return a.month > b.month;
    else if (a.day != b.day)
        return a.day > b.day;
    else
        return false;
}

bool Date::DateEqual(const Date& a, const Date& b)
{
    return a.year == b.year && a.month == b.month && a.day == b.day;
}


///////////////////////////////////////////////////////////////////////////////////
//HistoryTraffic
unsigned __int64 HistoryTraffic::kBytes() const
{
    return up_kBytes + down_kBytes;
}


///////////////////////////////////////////////////////////////////////////////////
wstring& DispStrings::Get(CommonDisplayItem item)
{
    return map_str[item];
}

const wstring& DispStrings::GetConst(CommonDisplayItem item) const
{
    auto iter = map_str.find(item);
    if (iter != map_str.end())
        return iter->second;
    static wstring empty_str;
    return empty_str;
}

const std::map<CommonDisplayItem, wstring>& DispStrings::GetAllItems() const
{
    return map_str;
}

bool DispStrings::operator==(const DispStrings& disp_str) const
{
    return map_str == disp_str.map_str;
}

bool DispStrings::IsInvalid() const
{
    return map_str.empty();
}

void DispStrings::Load(const std::wstring& plugin_id, const std::wstring& disp_str)
{
    auto plugin = theApp.m_plugins.GetItemById(plugin_id);
    if (plugin != nullptr)
    {
        map_str[plugin] = disp_str;
    }
}

///////////////////////////////////////////////////////////////////////////////////
bool FontInfo::operator==(const FontInfo& a) const
{
    return name == a.name && size == a.size && bold == a.bold && italic == a.italic
        && underline == a.underline && strike_out == a.strike_out;
}

///////////////////////////////////////////////////////////////////////////////////
bool StringSet::Contains(const std::wstring& str) const
{
    return string_set.count(str) != 0;
}

void StringSet::SetStrContained(const std::wstring& str, bool contained)
{
    if (contained)
        string_set.insert(str);
    else
        string_set.erase(str);
}

void StringSet::FromString(const std::wstring& str)
{
    std::vector<std::wstring> item_vect;
    CCommon::StringSplit(str, L',', item_vect);
    string_set.clear();
    for (const auto& i : item_vect)
        string_set.insert(i);
}

std::wstring StringSet::ToString() const
{
    std::vector<std::wstring> item_vect;
    for (const auto& i : string_set)
        item_vect.push_back(i);
    std::wstring item_str;
    for (const auto& i : item_vect)
    {
        item_str += i;
        item_str += L',';
    }
    if (!item_str.empty())
        item_str.pop_back();
    return item_str;
}

void StringSet::FromVector(const std::vector<std::wstring>& vec)
{
    string_set.clear();
    for (const auto& str : vec)
        string_set.insert(str);
}

std::vector<std::wstring> StringSet::ToVector() const
{
    std::vector<std::wstring> vec;
    for (const auto& str : string_set)
        vec.push_back(str);
    return vec;
}

std::set<std::wstring>& StringSet::data()
{
    return string_set;
}

///////////////////////////////////////////////////////////////////////////////////
bool SkinSettingData::IsEmpty() const
{
    return font.name.IsEmpty() && disp_str.GetAllItems().empty() && text_colors.empty();
}

bool SkinSettingData::operator==(const SkinSettingData& a) const
{
    return font == a.font && disp_str == a.disp_str && text_colors == a.text_colors && specify_each_item_color == a.specify_each_item_color;
}

///////////////////////////////////////////////////////////////////////////////////
void MainWndSettingData::FormSkinSettingData(const SkinSettingData& sking_setting_data)
{
    font = sking_setting_data.font;
    disp_str = sking_setting_data.disp_str;
    text_colors = sking_setting_data.text_colors;
    specify_each_item_color = sking_setting_data.specify_each_item_color;
}

SkinSettingData MainWndSettingData::ToSkinSettingData() const
{
    SkinSettingData sking_setting_data;
    sking_setting_data.font = font;
    sking_setting_data.disp_str = disp_str;
    sking_setting_data.text_colors = text_colors;
    sking_setting_data.specify_each_item_color = specify_each_item_color;
    return sking_setting_data;
}

///////////////////////////////////////////////////////////////////////////////////
bool TaskBarSettingData::IsTaskbarTransparent() const
{
    if (CWindowsSettingHelper::IsWindows10LightTheme() || theApp.m_win_version.IsWindows8Or8point1() || theApp.IsWindows11Taskbar())
        return (transparent_color == back_color);
    else
        return transparent_color == 0;
}

void TaskBarSettingData::SetTaskabrTransparent(bool transparent)
{
    if (transparent)
    {
        if (CWindowsSettingHelper::IsWindows10LightTheme() || theApp.m_win_version.IsWindows8Or8point1() || theApp.IsWindows11Taskbar())
        {
            //浅色模式下要设置任务栏窗口透明，只需将透明色设置成和背景色一样即可
            CCommon::TransparentColorConvert(back_color);
            transparent_color = back_color;
        }
        else
        {
            //深色模式下，背景色透明将透明色设置成黑色
            transparent_color = 0;
        }
    }
    else
    {
        //要设置任务栏窗口不透明，只需将透明色设置成和背景色不一样即可
        if (back_color != TASKBAR_TRANSPARENT_COLOR1)
            transparent_color = TASKBAR_TRANSPARENT_COLOR1;
        else
            transparent_color = TASKBAR_TRANSPARENT_COLOR2;
    }
}

void TaskBarSettingData::ValidItemSpace()
{
    if (item_space < 0)
        item_space = 0;
    if (item_space > 32)
        item_space = 32;
}

void TaskBarSettingData::ValidVerticalMargin()
{
    if (vertical_margin < -10)
        vertical_margin = -10;
    if (vertical_margin > 10)
        vertical_margin = 10;
}

void TaskBarSettingData::ValidWindowOffsetTop()
{
    if (window_offset_top < -20)
        window_offset_top = -20;
    if (window_offset_top > 20)
        window_offset_top = 20;
}

void TaskBarSettingData::ValidWindowOffsetLeft()
{
    if (window_offset_left < -800)
        window_offset_left = -800;
    if (window_offset_left > 800)
        window_offset_left = 800;
}

DisplayItemSet TaskBarSettingData::GetDisplayItemForDisplay(int display_index) const
{
    auto iter = display_item_per_display.find(display_index);
    if (iter != display_item_per_display.end())
    {
        DisplayItemSet display_item_set;
        display_item_set.FromInt(iter->second);
        return display_item_set;
    }
    return display_item;
}

void TaskBarSettingData::SetDisplayItemForDisplay(int display_index, const DisplayItemSet& display_item_value)
{
    display_item_per_display[display_index] = display_item_value.ToInt();
    if (display_index == 0)
        display_item = display_item_value;
}

int TaskBarSettingData::GetItemSpaceForDisplay(int display_index) const
{
    auto iter = item_space_per_display.find(display_index);
    if (iter != item_space_per_display.end())
        return iter->second;
    return item_space;
}

void TaskBarSettingData::SetItemSpaceForDisplay(int display_index, int value)
{
    int clamped_value = value;
    if (clamped_value < 0)
        clamped_value = 0;
    if (clamped_value > 32)
        clamped_value = 32;

    item_space_per_display[display_index] = clamped_value;
    if (display_index == 0)
        item_space = clamped_value;
}

int TaskBarSettingData::GetVerticalMarginForDisplay(int display_index) const
{
    auto iter = vertical_margin_per_display.find(display_index);
    if (iter != vertical_margin_per_display.end())
        return iter->second;
    return vertical_margin;
}

void TaskBarSettingData::SetVerticalMarginForDisplay(int display_index, int value)
{
    int clamped_value = value;
    if (clamped_value < -10)
        clamped_value = -10;
    if (clamped_value > 10)
        clamped_value = 10;

    vertical_margin_per_display[display_index] = clamped_value;
    if (display_index == 0)
        vertical_margin = clamped_value;
}

bool TaskBarSettingData::IsSpeedShortModeForDisplay(int display_index) const
{
    auto iter = speed_short_mode_per_display.find(display_index);
    if (iter != speed_short_mode_per_display.end())
        return iter->second;
    return speed_short_mode;
}

void TaskBarSettingData::SetSpeedShortModeForDisplay(int display_index, bool speed_short_mode_value)
{
    speed_short_mode_per_display[display_index] = speed_short_mode_value;
    if (display_index == 0)
        speed_short_mode = speed_short_mode_value;
}

bool TaskBarSettingData::IsValueRightAlignForDisplay(int display_index) const
{
    auto iter = value_right_align_per_display.find(display_index);
    if (iter != value_right_align_per_display.end())
        return iter->second;
    return value_right_align;
}

void TaskBarSettingData::SetValueRightAlignForDisplay(int display_index, bool value_right_align_value)
{
    value_right_align_per_display[display_index] = value_right_align_value;
    if (display_index == 0)
        value_right_align = value_right_align_value;
}

int TaskBarSettingData::GetDigitsNumberForDisplay(int display_index) const
{
    auto iter = digits_number_per_display.find(display_index);
    if (iter != digits_number_per_display.end())
    {
        int value = iter->second;
        if (value < 3)
            value = 3;
        if (value > 7)
            value = 7;
        return value;
    }

    int value = digits_number;
    if (value < 3)
        value = 3;
    if (value > 7)
        value = 7;
    return value;
}

void TaskBarSettingData::SetDigitsNumberForDisplay(int display_index, int digits_number_value)
{
    int clamped_value = digits_number_value;
    if (clamped_value < 3)
        clamped_value = 3;
    if (clamped_value > 7)
        clamped_value = 7;

    digits_number_per_display[display_index] = clamped_value;
    if (display_index == 0)
        digits_number = clamped_value;
}

bool TaskBarSettingData::IsHorizontalArrangeForDisplay(int display_index) const
{
    auto iter = horizontal_arrange_per_display.find(display_index);
    if (iter != horizontal_arrange_per_display.end())
        return iter->second;
    return horizontal_arrange;
}

void TaskBarSettingData::SetHorizontalArrangeForDisplay(int display_index, bool horizontal_arrange_value)
{
    horizontal_arrange_per_display[display_index] = horizontal_arrange_value;
    if (display_index == 0)
        horizontal_arrange = horizontal_arrange_value;
}

bool TaskBarSettingData::IsSeparateValueUnitWithSpaceForDisplay(int display_index) const
{
    auto iter = separate_value_unit_with_space_per_display.find(display_index);
    if (iter != separate_value_unit_with_space_per_display.end())
        return iter->second;
    return separate_value_unit_with_space;
}

void TaskBarSettingData::SetSeparateValueUnitWithSpaceForDisplay(int display_index, bool separate_value_unit_with_space_value)
{
    separate_value_unit_with_space_per_display[display_index] = separate_value_unit_with_space_value;
    if (display_index == 0)
        separate_value_unit_with_space = separate_value_unit_with_space_value;
}

bool TaskBarSettingData::IsShowToolTipForDisplay(int display_index) const
{
    auto iter = show_tool_tip_per_display.find(display_index);
    if (iter != show_tool_tip_per_display.end())
        return iter->second;
    return show_tool_tip;
}

void TaskBarSettingData::SetShowToolTipForDisplay(int display_index, bool show_tool_tip_value)
{
    show_tool_tip_per_display[display_index] = show_tool_tip_value;
    if (display_index == 0)
        show_tool_tip = show_tool_tip_value;
}

MemoryDisplay TaskBarSettingData::GetMemoryDisplayForDisplay(int display_index) const
{
    auto iter = memory_display_per_display.find(display_index);
    if (iter != memory_display_per_display.end())
    {
        int value = iter->second;
        if (value < static_cast<int>(MemoryDisplay::USAGE_PERCENTAGE) || value > static_cast<int>(MemoryDisplay::MEMORY_AVAILABLE))
            return memory_display;
        return static_cast<MemoryDisplay>(value);
    }
    return memory_display;
}

void TaskBarSettingData::SetMemoryDisplayForDisplay(int display_index, MemoryDisplay memory_display_value)
{
    int value = static_cast<int>(memory_display_value);
    if (value < static_cast<int>(MemoryDisplay::USAGE_PERCENTAGE))
        value = static_cast<int>(MemoryDisplay::USAGE_PERCENTAGE);
    if (value > static_cast<int>(MemoryDisplay::MEMORY_AVAILABLE))
        value = static_cast<int>(MemoryDisplay::MEMORY_AVAILABLE);

    memory_display_per_display[display_index] = value;
    if (display_index == 0)
        memory_display = static_cast<MemoryDisplay>(value);
}

int TaskBarSettingData::GetWindowOffsetTopForDisplay(int display_index) const
{
    auto iter = window_offset_top_per_display.find(display_index);
    if (iter != window_offset_top_per_display.end())
        return iter->second;
    return window_offset_top;
}

void TaskBarSettingData::SetWindowOffsetTopForDisplay(int display_index, int value)
{
    int clamped_value = value;
    if (clamped_value < -20)
        clamped_value = -20;
    if (clamped_value > 20)
        clamped_value = 20;

    window_offset_top_per_display[display_index] = clamped_value;
    if (display_index == 0)
        window_offset_top = clamped_value;
}

int TaskBarSettingData::GetWindowOffsetLeftForDisplay(int display_index) const
{
    auto iter = window_offset_left_per_display.find(display_index);
    if (iter != window_offset_left_per_display.end())
        return iter->second;
    return window_offset_left;
}

void TaskBarSettingData::SetWindowOffsetLeftForDisplay(int display_index, int value)
{
    int clamped_value = value;
    if (clamped_value < -800)
        clamped_value = -800;
    if (clamped_value > 800)
        clamped_value = 800;

    window_offset_left_per_display[display_index] = clamped_value;
    if (display_index == 0)
        window_offset_left = clamped_value;
}

bool TaskBarSettingData::IsTaskbarWndSnapForDisplay(int display_index) const
{
    auto iter = tbar_wnd_snap_per_display.find(display_index);
    if (iter != tbar_wnd_snap_per_display.end())
        return iter->second;
    return tbar_wnd_snap;
}

void TaskBarSettingData::SetTaskbarWndSnapForDisplay(int display_index, bool snap)
{
    tbar_wnd_snap_per_display[display_index] = snap;
    if (display_index == 0)
        tbar_wnd_snap = snap;
}

bool TaskBarSettingData::IsAvoidOverlapWithWidgetsForDisplay(int display_index) const
{
    auto iter = avoid_overlap_with_widgets_per_display.find(display_index);
    if (iter != avoid_overlap_with_widgets_per_display.end())
        return iter->second;
    return avoid_overlap_with_widgets;
}

void TaskBarSettingData::SetAvoidOverlapWithWidgetsForDisplay(int display_index, bool avoid_overlap)
{
    avoid_overlap_with_widgets_per_display[display_index] = avoid_overlap;
    if (display_index == 0)
        avoid_overlap_with_widgets = avoid_overlap;
}

int TaskBarSettingData::GetTaskbarLeftSpaceForDisplay(int display_index) const
{
    auto iter = taskbar_left_space_win11_per_display.find(display_index);
    if (iter != taskbar_left_space_win11_per_display.end())
        return iter->second;
    return taskbar_left_space_win11;
}

void TaskBarSettingData::SetTaskbarLeftSpaceForDisplay(int display_index, int value)
{
    int clamped_value = value;
    if (clamped_value < 0)
        clamped_value = 0;
    if (clamped_value > 300)
        clamped_value = 300;

    taskbar_left_space_win11_per_display[display_index] = clamped_value;
    if (display_index == 0)
        taskbar_left_space_win11 = clamped_value;
}

unsigned __int64 TaskBarSettingData::GetNetspeedFigureMaxValueInBytes() const
{
    if (netspeed_figure_max_value_unit == 0)        //单位为KB
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024;
    else
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024 * 1024;
}

COLORREF TaskBarSettingData::GetUsageGraphColor() const
{
    if (graph_color_following_system)
    {
        COLORREF theme_color = theApp.GetThemeColor();
        //转换为HLS
        double h, l, s;
        CDrawingManager::RGBtoHSL(theme_color, &h, &s, &l);
        //根据当前系统深浅色模式指定亮度
        if (theApp.m_last_light_mode)
        {
            //浅色任务栏，将亮度设为0.7
            l = 0.7;
        }
        else
        {
            //深色任务栏，将亮度设为0.4
            l = 0.4;
        }
        //转换回RGB
        COLORREF graph_color = CDrawingManager::HLStoRGB_ONE(h, l, s);
        return graph_color;
    }
    else
    {
        return status_bar_color;
    }
}

wstring LanguageInfo::toConfigString() const
{
    return bcp_47 + L"|" + display_name + L"|" + translator;
}

void LanguageInfo::fromConfigString(const wstring& config_str)
{
    std::vector<wstring> parts;
    CCommon::StringSplit(config_str, L'|', parts, false);
    if (parts.size() >= 1)
    {
        bcp_47 = parts[0];
    }
    if (parts.size() >= 2)
    {
        display_name = parts[1];
    }
    if (parts.size() >= 3)
    {
        translator = parts[2];
    }
    language_id = LocaleNameToLCID(bcp_47.c_str(), 0);
}
