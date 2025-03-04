#pragma once
#include "PluginInterface.h"

//内置的显示的项目
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
    TDI_MAIN_BOARD_TEMP = 1 << 8,
    TDI_HDD_USAGE = 1 << 9,
    TDI_TOTAL_SPEED = 1 << 10,
    TDI_CPU_FREQ = 1 << 11,
    TDI_TODAY_TRAFFIC = 1 << 12
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
