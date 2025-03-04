#include "stdafx.h"
#include "TaskbarItemOrderHelper.h"
#include "Common.h"
#include "TrafficMonitor.h"

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
