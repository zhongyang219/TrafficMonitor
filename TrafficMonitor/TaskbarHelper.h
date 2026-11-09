#pragma once
#include <vector>
#include <afxwin.h>

//单个显示器的信息
struct MonitorDisplayInfo
{
    int index{ 0 };              //显示器的编号（从0开始，其中0为主显示器）
    HMONITOR hMonitor{ nullptr };
    HWND hTaskbar{ nullptr };    //该显示器上的任务栏窗口句柄（如果该显示器上没有任务栏，则为nullptr）
    CString device_name;        //设备名，如\\.\DISPLAY1
    CString display_name;       //显示器的名称（从EDID信息中读取，可能为空）
    CString resolution;         //分辨率，如2560 x 1440
    bool is_primary{ false };    //是否为主显示器
    bool has_taskbar{ false };   //该显示器上是否有任务栏
    CRect rect;                 //显示器在虚拟桌面坐标中的矩形区域
};

class CTaskbarHelper
{
public:
    //获取所有副显示器上的任务栏，并确保按显示器的顺序
    static void GetAllSecondaryDisplayTaskbar(std::vector<HWND>& secondary_taskbars);

    //获取显示器数量
    static int GetDisplayNum();

    //获取副显示器任务栏的数量
    static int GetSecondaryTaskbarNum();

    //枚举系统中所有的显示器，同时获取每个显示器的任务栏句柄、分辨率和名称等信息
    //显示器的顺序为：主显示器排在最前面，其余显示器按照在虚拟桌面中的位置排序
    static void GetAllDisplayInfo(std::vector<MonitorDisplayInfo>& displays);

    //获取显示器的描述文字，如“主显示器 (2560 x 1440) - DELL U2720Q”
    static CString GetDisplayDescription(const MonitorDisplayInfo& display);

    //根据编号查找显示器信息，如果找不到则返回nullptr
    static const MonitorDisplayInfo* FindDisplayByIndex(const std::vector<MonitorDisplayInfo>& displays, int index);
};
