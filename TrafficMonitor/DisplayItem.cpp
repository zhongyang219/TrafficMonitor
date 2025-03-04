#include "stdafx.h"
#include "DisplayItem.h"
#include "Common.h"
#include "TrafficMonitor.h"

CommonDisplayItem::CommonDisplayItem(DisplayItem item)
{
    is_plugin = false;
    item_type = item;
    plugin_item = nullptr;
}

CommonDisplayItem::CommonDisplayItem(IPluginItem* item)
{
    is_plugin = true;
    plugin_item = item;
}

bool CommonDisplayItem::operator<(const CommonDisplayItem& item) const
{
    if (is_plugin && !item.is_plugin)
        return false;
    else if (!is_plugin && item.is_plugin)
        return true;
    else if (!is_plugin)
        return item_type < item.item_type;
    else
        return theApp.m_plugins.GetItemIndex(plugin_item) < theApp.m_plugins.GetItemIndex(item.plugin_item);
}

bool CommonDisplayItem::operator==(const CommonDisplayItem& item) const
{
    if (is_plugin != item.is_plugin)
        return false;
    else if (!is_plugin)
        return item_type == item.item_type;
    else
        return plugin_item == item.plugin_item;
}

CString CommonDisplayItem::GetItemName() const
{
    if (is_plugin)
    {
        if (plugin_item != nullptr)
            return plugin_item->GetItemName();
        else
            return CString();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return CCommon::LoadText(IDS_UPLOAD);
        case TDI_DOWN: return CCommon::LoadText(IDS_DOWNLOAD);
        case TDI_TOTAL_SPEED: return CCommon::LoadText(IDS_TOTAL_NET_SPEED);
        case TDI_CPU: return CCommon::LoadText(IDS_CPU_USAGE);
        case TDI_MEMORY: return CCommon::LoadText(IDS_MEMORY_USAGE);
        case TDI_GPU_USAGE: return CCommon::LoadText(IDS_GPU_USAGE);
        case TDI_CPU_TEMP: return CCommon::LoadText(IDS_CPU_TEMPERATURE);
        case TDI_GPU_TEMP: return CCommon::LoadText(IDS_GPU_TEMPERATURE);
        case TDI_HDD_TEMP: return CCommon::LoadText(IDS_HDD_TEMPERATURE);
        case TDI_MAIN_BOARD_TEMP: return CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
        case TDI_HDD_USAGE: return CCommon::LoadText(IDS_HDD_USAGE);
        case TDI_CPU_FREQ: return CCommon::LoadText(IDS_CPU_FREQ);
        case TDI_TODAY_TRAFFIC: return CCommon::LoadText(IDS_TRAFFIC_USED);
        default:
            ASSERT(false);
            break;
        }
    }
    return CString();
}

std::wstring CommonDisplayItem::DefaultString(bool is_main_window) const
{
    std::wstring default_text;
    if (is_plugin)
    {
        default_text = plugin_item->GetItemLableText();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_UPLOAD_DISP, _T(": "));
            else
                default_text = _T("↑: ");
            break;
        case TDI_DOWN:
            if (is_main_window)
                default_text = CCommon::LoadText(IDS_DOWNLOAD_DISP, _T(": "));
            else
                default_text = _T("↓: ");
            break;
        case TDI_TOTAL_SPEED:
            default_text = _T("↑↓: ");
            break;
        case TDI_TODAY_TRAFFIC:
            default_text = CCommon::LoadText(IDS_TRAFFIC_USED, _T(": "));
            break;
        case TDI_CPU:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_TEMP:
            default_text = _T("CPU: ");
            break;
        case TDI_CPU_FREQ:
            default_text = CCommon::LoadText(IDS_CPU_FREQ, _T(": "));
            break;
        case TDI_MEMORY:
            default_text = CCommon::LoadText(IDS_MEMORY_DISP, _T(": "));
            break;
        case TDI_GPU_USAGE:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_GPU_TEMP:
            default_text = CCommon::LoadText(IDS_GPU_DISP, _T(": "));
            break;
        case TDI_HDD_TEMP:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        case TDI_MAIN_BOARD_TEMP:
            default_text = CCommon::LoadText(IDS_MAINBOARD_DISP, _T(": "));
            break;
        case TDI_HDD_USAGE:
            default_text = CCommon::LoadText(IDS_HDD_DISP, _T(": "));
            break;
        default:
            ASSERT(false);
            break;
        }
    }

    return default_text;
}

const wchar_t* CommonDisplayItem::GetItemIniKeyName() const
{
    if (is_plugin)
    {
        return plugin_item->GetItemId();
    }
    else
    {
        switch (item_type)
        {
        case TDI_UP: return L"up_string";
        case TDI_DOWN: return L"down_string";
        case TDI_CPU: return L"cpu_string";
        case TDI_MEMORY: return L"memory_string";
        case TDI_GPU_USAGE: return L"gpu_string";
        case TDI_CPU_TEMP: return L"cpu_temp_string";
        case TDI_GPU_TEMP: return L"gpu_temp_string";
        case TDI_HDD_TEMP: return L"hdd_temp_string";
        case TDI_MAIN_BOARD_TEMP: return L"main_board_temp_string";
        case TDI_HDD_USAGE: return L"hdd_string";
        case TDI_TOTAL_SPEED: return L"total_speed_string";
        case TDI_CPU_FREQ: return L"cpu_freq_string";
        case TDI_TODAY_TRAFFIC: return L"today_traffic_string";
        }
        ASSERT(FALSE);
        return L"";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////
DisplayItemSet::DisplayItemSet(std::initializer_list<DisplayItem> items)
    : data(items)
{
}

void DisplayItemSet::Add(DisplayItem item)
{
    data.insert(item);
}

void DisplayItemSet::Remove(DisplayItem item)
{
    data.erase(item);
}

bool DisplayItemSet::Contains(DisplayItem item) const
{
    auto iter = data.find(item);
    return iter != data.end();
}

int DisplayItemSet::ToInt() const
{
    int value = 0;
    //将set中的枚举值转换成int的每个bit位
    for (const auto& item : data)
    {
        if (item <= 31)     //int左移不超过31位
            value |= (1 << item);
    }
    return value;
}

void DisplayItemSet::FromInt(int value)
{
    data.clear();
    //将int的每个bit位转换成set中的枚举值
    for (const auto& item : AllDisplayItems)
    {
        if (value <= 31 && (value & (1 << item)))
            data.insert(item);
    }
}

bool DisplayItemSet::IsEmpty() const
{
    return data.empty();
}
