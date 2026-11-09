#include "stdafx.h"
#include "TaskbarHelper.h"
#include "Common.h"
#include <algorithm>

// 存储显示器信息
struct MonitorInfo
{
    HMONITOR hMonitor;
    CRect rect;
};

// 存储任务栏信息
struct TaskbarInfo
{
    HWND hwnd;
    CRect rect;
};

std::vector<MonitorInfo> monitors;
std::vector<TaskbarInfo> taskbars;

// 枚举显示器的回调函数
static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MonitorInfo info;
    info.hMonitor = hMonitor;
    info.rect = *lprcMonitor;
    if (!info.rect.IsRectEmpty())
        monitors.push_back(info);
    else
        ASSERT(FALSE);
    return TRUE;
}

// 枚举窗口的回调函数
static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    TCHAR className[256];
    GetClassName(hwnd, className, 256);

    // 检查是否是副显示器的任务栏
    if (_tcscmp(className, _T("Shell_SecondaryTrayWnd")) == 0)
    {
        TaskbarInfo info;
        info.hwnd = hwnd;
        GetWindowRect(hwnd, &info.rect);
        taskbars.push_back(info);
    }

    return TRUE;
}

// 比较函数：按显示器的左上角坐标排序
static bool CompareTaskbarByMonitorOrder(const TaskbarInfo& a, const TaskbarInfo& b)
{
    // 遍历所有显示器
    for (const auto& monitor : monitors)
    {
        // 检查任务栏 a 是否在当前显示器内
        bool aInMonitor = (a.rect.left >= monitor.rect.left && a.rect.top >= monitor.rect.top);
        // 检查任务栏 b 是否在当前显示器内
        bool bInMonitor = (b.rect.left >= monitor.rect.left && b.rect.top >= monitor.rect.top);

        // 如果 a 在当前显示器内而 b 不在，则 a 应该在 b 前面
        if (aInMonitor && !bInMonitor)
            return true;
        // 如果 b 在当前显示器内而 a 不在，则 b 应该在 a 前面
        if (bInMonitor && !aInMonitor)
            return false;
        // 如果 a 和 b 都在当前显示器内，则按任务栏的位置排序
        if (aInMonitor && bInMonitor)
        {
            if (a.rect.left != b.rect.left)
                return a.rect.left < b.rect.left;
            return a.rect.top < b.rect.top;
        }
    }
    // 默认情况下，a 和 b 相等
    return false;
}

// 计算两个矩形的重叠面积
static long long GetOverlapArea(const CRect& a, const CRect& b)
{
    int left = (std::max)(a.left, b.left);
    int top = (std::max)(a.top, b.top);
    int right = (std::min)(a.right, b.right);
    int bottom = (std::min)(a.bottom, b.bottom);
    if (right <= left || bottom <= top)
        return 0;

    return static_cast<long long>(right - left) * static_cast<long long>(bottom - top);
}

// 枚举显示器的回调函数（获取所有显示器的详细信息）
static BOOL CALLBACK EnumDisplayInfoProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    std::vector<MonitorDisplayInfo>* p_displays = reinterpret_cast<std::vector<MonitorDisplayInfo>*>(dwData);
    if (lprcMonitor == nullptr)
        return TRUE;

    MonitorDisplayInfo info;
    info.hMonitor = hMonitor;
    info.rect = *lprcMonitor;

    MONITORINFOEX monitor_info{};
    monitor_info.cbSize = sizeof(monitor_info);
    if (GetMonitorInfo(hMonitor, &monitor_info))
    {
        info.device_name = monitor_info.szDevice;
        info.is_primary = ((monitor_info.dwFlags & MONITORINFOF_PRIMARY) != 0);
        //rcMonitor为整个显示区域的矩形（包含任务栏所在的区域），用它来计算分辨率
        info.resolution.Format(_T("%d x %d"),
            monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
            monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);
        //获取显示器的名称（EDID信息中的名称）
        DISPLAY_DEVICE display_device{};
        display_device.cb = sizeof(display_device);
        if (EnumDisplayDevices(monitor_info.szDevice, 0, &display_device, 0))
            info.display_name = display_device.DeviceString;
    }

    p_displays->push_back(info);
    return TRUE;
}

// 枚举任务栏窗口的回调函数（同时枚举主显示器和副显示器的任务栏）
static BOOL CALLBACK EnumTaskbarWndProc(HWND hwnd, LPARAM lParam)
{
    std::vector<TaskbarInfo>* p_taskbars = reinterpret_cast<std::vector<TaskbarInfo>*>(lParam);

    TCHAR className[256];
    GetClassName(hwnd, className, 256);

    //Shell_TrayWnd为主显示器的任务栏，Shell_SecondaryTrayWnd为副显示器的任务栏
    if (_tcscmp(className, _T("Shell_TrayWnd")) == 0 || _tcscmp(className, _T("Shell_SecondaryTrayWnd")) == 0)
    {
        TaskbarInfo info;
        info.hwnd = hwnd;
        GetWindowRect(hwnd, &info.rect);
        p_taskbars->push_back(info);
    }

    return TRUE;
}

void CTaskbarHelper::GetAllSecondaryDisplayTaskbar(std::vector<HWND>& secondary_taskbars)
{
    monitors.clear();
    taskbars.clear();
    secondary_taskbars.clear();

    // 获取所有显示器信息
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

    // 获取所有任务栏句柄
    EnumWindows(EnumWindowsProc, 0);

    // 按显示器顺序对任务栏排序
    std::sort(taskbars.begin(), taskbars.end(), CompareTaskbarByMonitorOrder);

    //保存任务栏句柄
    for (const auto& taskbar : taskbars)
    {
        secondary_taskbars.push_back(taskbar.hwnd);
    }
}

int CTaskbarHelper::GetDisplayNum()
{
    monitors.clear();
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
    return static_cast<int>(monitors.size());
}

int CTaskbarHelper::GetSecondaryTaskbarNum()
{
    taskbars.clear();
    EnumWindows(EnumWindowsProc, 0);
    return static_cast<int>(taskbars.size());
}

void CTaskbarHelper::GetAllDisplayInfo(std::vector<MonitorDisplayInfo>& displays)
{
    displays.clear();

    //枚举所有显示器
    EnumDisplayMonitors(NULL, NULL, EnumDisplayInfoProc, reinterpret_cast<LPARAM>(&displays));

    //枚举所有任务栏窗口
    std::vector<TaskbarInfo> taskbar_list;
    EnumWindows(EnumTaskbarWndProc, reinterpret_cast<LPARAM>(&taskbar_list));

    //将任务栏窗口和显示器匹配：任务栏窗口位于某个显示器的区域内
    for (auto& display : displays)
    {
        long long max_overlap_area = 0;
        for (const auto& taskbar : taskbar_list)
        {
            long long overlap_area = GetOverlapArea(taskbar.rect, display.rect);
            if (overlap_area > max_overlap_area)
            {
                max_overlap_area = overlap_area;
                display.hTaskbar = taskbar.hwnd;
            }
        }
        display.has_taskbar = (display.hTaskbar != nullptr);
    }

    //排序：主显示器排在最前面，其余显示器按照在虚拟桌面中的位置排序
    std::sort(displays.begin(), displays.end(), [](const MonitorDisplayInfo& a, const MonitorDisplayInfo& b) {
        if (a.is_primary != b.is_primary)
            return a.is_primary;
        if (a.rect.left != b.rect.left)
            return a.rect.left < b.rect.left;
        return a.rect.top < b.rect.top;
    });

    //重新编号，保证编号和数组下标一致
    for (size_t i = 0; i < displays.size(); i++)
    {
        displays[i].index = static_cast<int>(i);
    }
}

CString CTaskbarHelper::GetDisplayDescription(const MonitorDisplayInfo& display)
{
    CString description;
    if (display.is_primary)
    {
        description = CCommon::LoadText(IDS_PRIMARY_DISPLAY);
    }
    else
    {
        //副显示器的编号从1开始（0为主显示器）
        description = CCommon::LoadTextFormat(IDS_SECONDARY_DISPLAY, { display.index });
    }

    //分辨率
    if (!display.resolution.IsEmpty())
    {
        description += _T(" (");
        description += display.resolution;
        description += _T(")");
    }

    //显示器名称
    if (!display.display_name.IsEmpty())
    {
        description += _T(" - ");
        description += display.display_name;
    }

    return description;
}

const MonitorDisplayInfo* CTaskbarHelper::FindDisplayByIndex(const std::vector<MonitorDisplayInfo>& displays, int index)
{
    if (index < 0 || index >= static_cast<int>(displays.size()))
        return nullptr;

    return &displays[index];
}
