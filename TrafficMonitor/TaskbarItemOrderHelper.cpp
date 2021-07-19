#include "stdafx.h"
#include "TaskbarItemOrderHelper.h"
#include "Common.h"

CTaskbarItemOrderHelper::CTaskbarItemOrderHelper()
{
    for (const auto& item : AllDisplayItems)
    {
        m_all_item_in_default_order.push_back(item);
    }
}

std::vector<DisplayItem> CTaskbarItemOrderHelper::GetAllDisplayItemsWithOrder() const
{
    std::vector<DisplayItem> items;
    for (auto i : m_item_order)
    {
        if (i >= 0 && i < static_cast<int>(m_all_item_in_default_order.size()))
            items.push_back(m_all_item_in_default_order[i]);
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

CString CTaskbarItemOrderHelper::GetItemDisplayName(DisplayItem item)
{
    switch (item)
    {
    case TDI_UP:
        return CCommon::LoadText(IDS_UPLOAD);
    case TDI_DOWN:
        return CCommon::LoadText(IDS_DOWNLOAD);
    case TDI_CPU:
        return CCommon::LoadText(IDS_CPU_USAGE);
    case TDI_MEMORY:
        return CCommon::LoadText(IDS_MEMORY_USAGE);
    case TDI_GPU_USAGE:
        return CCommon::LoadText(IDS_GPU_USAGE);
    case TDI_CPU_TEMP:
        return CCommon::LoadText(IDS_CPU_TEMPERATURE);
    case TDI_GPU_TEMP:
        return CCommon::LoadText(IDS_GPU_TEMPERATURE);
    case TDI_HDD_TEMP:
        return CCommon::LoadText(IDS_HDD_TEMPERATURE);
    case TDI_MAIN_BOARD_TEMP:
        return CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
    default:
        break;
    }
    return CString();
}

void CTaskbarItemOrderHelper::NormalizeItemOrder()
{
    //检查是否有超出范围的序号
    int item_num = static_cast<int>(AllDisplayItems.size());
    for (auto iter = m_item_order.begin(); iter != m_item_order.end();)
    {
        if (*iter < 0 || *iter >= item_num)
            iter = m_item_order.erase(iter);
        else
            ++iter;
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
