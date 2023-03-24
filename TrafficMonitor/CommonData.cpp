﻿#include "stdafx.h"
#include "CommonData.h"
#include "Common.h"
#include "CalendarHelper.h"
#include "TrafficMonitor.h"

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
    switch (theApp.m_cfg_data.m_first_day_of_week)
    {
        case FirstDayOfWeek::SATURDAY:
            days += week_day;
            break;
        case FirstDayOfWeek::SUNDAY:
            days += (week_day - 1);
            break;
        case FirstDayOfWeek::MONDAY:
            days += (week_day - 2);
            break;
        default:
            break;
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

const std::map<CommonDisplayItem, wstring>& DispStrings::GetAllItems() const
{
    return map_str;
}

void DispStrings::operator=(const DispStrings& disp_str)
{
    map_str = disp_str.map_str;
    //如果赋值的字符串是定义的无效字符串，则不赋值
    for (auto& iter = map_str.begin(); iter != map_str.end(); ++iter)
    {
        if (iter->second == NONE_STR)
            iter->second.clear();
    }
}

bool DispStrings::IsInvalid() const
{
    for (auto& iter = map_str.begin(); iter != map_str.end(); ++iter)
    {
        if (iter->second == NONE_STR)
            return true;
    }
    return false;
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

void TaskBarSettingData::ValidItemSpace()
{
    if (item_space < 0)
        item_space = 0;
    if (item_space > 32)
        item_space = 32;
}

void TaskBarSettingData::ValidWindowOffsetTop()
{
    if (window_offset_top < -5)
        window_offset_top = -5;
    if (window_offset_top > 20)
        window_offset_top = 20;
}

void TaskBarSettingData::ValidVerticalMargin()
{
    if (vertical_margin < -10)
        vertical_margin = -10;
    if (vertical_margin > 10)
        vertical_margin = 10;
}

unsigned __int64 TaskBarSettingData::GetNetspeedFigureMaxValueInBytes() const
{
    if (netspeed_figure_max_value_unit == 0)        //单位为KB
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024;
    else
        return static_cast<unsigned __int64>(netspeed_figure_max_value) * 1024 * 1024;
}
