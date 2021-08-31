#include "stdafx.h"
#include "CommonData.h"
#include "Common.h"

///////////////////////////////////////////////////////////////////////////////////
//HistoryTraffic
unsigned __int64 HistoryTraffic::kBytes() const
{
    return up_kBytes + down_kBytes;
}

bool HistoryTraffic::DateGreater(const HistoryTraffic& a, const HistoryTraffic& b)
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

bool HistoryTraffic::DateEqual(const HistoryTraffic& a, const HistoryTraffic& b)
{
    return a.year == b.year && a.month == b.month && a.day == b.day;
}


///////////////////////////////////////////////////////////////////////////////////
wstring& DispStrings::Get(DisplayItem item)
{
    return map_str[item];
}

const std::map<DisplayItem, wstring>& DispStrings::GetAllItems() const
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


///////////////////////////////////////////////////////////////////////////////////
//TaskBarSettingData
bool TaskBarSettingData::IsPluginItemDisplayed(const std::wstring& id) const
{
    return plugin_display_item.count(id) != 0;
}

void TaskBarSettingData::SetPluginItemDisplayed(const std::wstring& id, bool displayed)
{
    if (displayed)
        plugin_display_item.insert(id);
    else
        plugin_display_item.erase(id);
}

void TaskBarSettingData::PluginDisplayItemFromString(const std::wstring& str)
{
    std::vector<std::wstring> plugin_display_item_vect;
    CCommon::StringSplit(str, L',', plugin_display_item_vect);
    plugin_display_item.clear();
    for (const auto& i : plugin_display_item_vect)
        plugin_display_item.insert(i);

}

std::wstring TaskBarSettingData::PluginDisplayItemToString() const
{
    std::vector<std::wstring> plugin_display_item_vect;
    for (const auto& i : plugin_display_item)
        plugin_display_item_vect.push_back(i);
    std::wstring plugin_display_item_str;
    for (const auto& i : plugin_display_item_vect)
    {
        plugin_display_item_str += i;
        plugin_display_item_str += L',';
    }
    if (!plugin_display_item_str.empty())
        plugin_display_item_str.pop_back();
    return plugin_display_item_str;
}
