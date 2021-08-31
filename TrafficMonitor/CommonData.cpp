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
