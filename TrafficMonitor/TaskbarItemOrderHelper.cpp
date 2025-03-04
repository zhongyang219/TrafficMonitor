#include "stdafx.h"
#include "TaskbarItemOrderHelper.h"
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
        if (is_main_window)
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

bool CommonDisplayItem::operator==(const CommonDisplayItem& item) const
{
    if (is_plugin != item.is_plugin)
        return false;
    else if (!is_plugin)
        return item_type == item.item_type;
    else
        return plugin_item == item.plugin_item;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

CTaskbarItemOrderHelper::CTaskbarItemOrderHelper(bool displayed_only)
    : m_displayed_only(displayed_only)
{
}

void CTaskbarItemOrderHelper::Init()
{
    for (const auto& item : theApp.m_plugins.AllDisplayItemsWithPlugins())
    {
        m_all_item_in_default_order.push_back(item);
    }
}

std::vector<CommonDisplayItem> CTaskbarItemOrderHelper::GetAllDisplayItemsWithOrder() const
{
    std::vector<CommonDisplayItem> items;
    for (auto i : m_item_order)
    {
        if (i >= 0 && i < static_cast<int>(m_all_item_in_default_order.size()))
        {
            if (m_displayed_only && !IsItemDisplayed(m_all_item_in_default_order[i]))
            {
                continue;
            }
            items.push_back(m_all_item_in_default_order[i]);
        }
    }

    return items;
}

void CTaskbarItemOrderHelper::FromString(const std::wstring& str)
{
    m_item_order.clear();
    std::vector<std::wstring> str_list;
    CCommon::StringSplit(str, L',', str_list);
    for (const auto& s : str_list)
    {
        m_item_order.push_back(_wtoi(s.c_str()));
    }
    NormalizeItemOrder();
}

std::wstring CTaskbarItemOrderHelper::ToString() const
{
    std::wstring result;
    for (int i : m_item_order)
    {
        result += std::to_wstring(i);
        result.push_back(L',');
    }
    if (!m_item_order.empty())
        result.pop_back();
    return result;
}

void CTaskbarItemOrderHelper::SetOrder(const vector<int>& item_order)
{
    m_item_order = item_order;
    NormalizeItemOrder();
}

const vector<int>& CTaskbarItemOrderHelper::GetItemOrderConst() const
{
    return m_item_order;
}

vector<int>& CTaskbarItemOrderHelper::GetItemOrder()
{
    return m_item_order;
}

CString CTaskbarItemOrderHelper::GetItemDisplayName(CommonDisplayItem item)
{
    return item.GetItemName();
}

bool CTaskbarItemOrderHelper::IsItemDisplayed(CommonDisplayItem item)
{
    bool displayed = true;
    if (!item.is_plugin)
    {
        if ((item == TDI_CPU_TEMP) && !theApp.m_general_data.IsHardwareEnable(HI_CPU))
            displayed = false;
        if ((item == TDI_GPU_TEMP || item == TDI_GPU_USAGE) && !theApp.m_general_data.IsHardwareEnable(HI_GPU))
            displayed = false;
        if ((item == TDI_HDD_TEMP || item == TDI_HDD_USAGE) && !theApp.m_general_data.IsHardwareEnable(HI_HDD))
            displayed = false;
        if (item == TDI_MAIN_BOARD_TEMP && !theApp.m_general_data.IsHardwareEnable(HI_MBD))
            displayed = false;
    }

    return displayed;
}

void CTaskbarItemOrderHelper::NormalizeItemOrder()
{
    //检查是否有超出范围的序号
    int item_num = static_cast<int>(theApp.m_plugins.AllDisplayItemsWithPlugins().size());
    for (auto iter = m_item_order.begin(); iter != m_item_order.end();)
    {
        if (*iter < 0 || *iter >= item_num)
            iter = m_item_order.erase(iter);
        else
            ++iter;
    }
    //删除不显示的序号
    if (m_displayed_only)
    {
        for (auto iter = m_item_order.begin(); iter != m_item_order.end();)
        {
            if (*iter >= 0 && *iter < static_cast<int>(m_all_item_in_default_order.size()))
            {
                CommonDisplayItem item = m_all_item_in_default_order[*iter];
                if (!IsItemDisplayed(item))
                {
                    iter = m_item_order.erase(iter);
                    continue;
                }
            }
            ++iter;
        }
    }
    //删除重复的序号
    CCommon::RemoveVectorDuplicateItem(m_item_order);
    //检查是否有缺少的序号
    for (int i = 0; i < item_num; i++)
    {
        auto iter = std::find(m_item_order.begin(), m_item_order.end(), i);
        if (iter == m_item_order.end())
            m_item_order.push_back(i);
    }
}
