#pragma once

//显示的项目
enum DisplayItem
{
    TDI_UP = 1 << 0,
    TDI_DOWN = 1 << 1,
    TDI_CPU = 1 << 2,
    TDI_MEMORY = 1 << 3,
    TDI_GPU_USAGE = 1 << 4,
    TDI_CPU_TEMP = 1 << 5,
    TDI_GPU_TEMP = 1 << 6,
    TDI_HDD_TEMP = 1 << 7,
    TDI_MAIN_BOARD_TEMP = 1 << 8
};

//所有显示项目的集合
const std::set<DisplayItem> AllDisplayItems{ TDI_UP, TDI_DOWN, TDI_CPU, TDI_MEMORY, TDI_GPU_USAGE, TDI_CPU_TEMP, TDI_GPU_TEMP, TDI_HDD_TEMP, TDI_MAIN_BOARD_TEMP };


class CTaskbarItemOrderHelper
{
public:
    CTaskbarItemOrderHelper();

    //根据设定的顺序获取任务窗口每个显示项目
    std::vector<DisplayItem> GetAllDisplayItemsWithOrder() const;

    void FromString(const std::wstring& str);
    std::wstring ToString() const;
    void SetOrder(const vector<int>& item_order);
    const vector<int>& GetItemOrder() const;

private:
    //规范m_item_order里的项目，如果m_item_order里有序号超过了显示项目的个数，则将其移除，并在后面添加缺少的项目的序号
    void NormalizeItemOrder();

private:
    vector<int> m_item_order;   //保存每个项目的顺序
    vector<DisplayItem> m_all_item_in_default_order;    //以默认顺序保存的所有显示项目
};
