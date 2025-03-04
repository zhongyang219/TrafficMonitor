#pragma once
#include "PluginInterface.h"

//内置的显示的项目
enum DisplayItem
{
    TDI_UP,
    TDI_DOWN,
    TDI_CPU,
    TDI_MEMORY,
    TDI_GPU_USAGE,
    TDI_CPU_TEMP,
    TDI_GPU_TEMP,
    TDI_HDD_TEMP,
    TDI_MAIN_BOARD_TEMP,
    TDI_HDD_USAGE,
    TDI_TOTAL_SPEED,
    TDI_CPU_FREQ,
    TDI_TODAY_TRAFFIC
};

//所有内置显示项目的集合
const std::set<DisplayItem> AllDisplayItems
{
    TDI_UP, TDI_DOWN, TDI_CPU, TDI_MEMORY
#ifndef WITHOUT_TEMPERATURE
    , TDI_GPU_USAGE, TDI_CPU_TEMP, TDI_GPU_TEMP, TDI_HDD_TEMP, TDI_MAIN_BOARD_TEMP, TDI_HDD_USAGE
#endif
    ,TDI_CPU_FREQ, TDI_TOTAL_SPEED, TDI_TODAY_TRAFFIC
};

//显示的项目
struct CommonDisplayItem
{
    bool is_plugin{};           //是否为插件项目
    DisplayItem item_type{};    //内建的显示项目
    IPluginItem* plugin_item{}; //插件显示项目

    CommonDisplayItem() {}
    CommonDisplayItem(DisplayItem item);
    CommonDisplayItem(IPluginItem* item);

    bool operator<(const CommonDisplayItem&) const;
    bool operator==(const CommonDisplayItem&) const;

    //获取显示项目的名称
    CString GetItemName() const;

    //获取一个显示项目的默认显示文本
    std::wstring DefaultString(bool is_main_window) const;

    //获取一个显示项目的显示文本保存在ini文件中的key的名称
    const wchar_t* GetItemIniKeyName() const;
};


class DisplayItemSet
{
public:
    DisplayItemSet(){}
    DisplayItemSet(std::initializer_list<DisplayItem> items);

    void Add(DisplayItem item);
    void Remove(DisplayItem item);
    bool Contains(DisplayItem item) const;

    int ToInt() const;
    void FromInt(int value);
    bool IsEmpty() const;

private:
    std::set<DisplayItem> data;
};
