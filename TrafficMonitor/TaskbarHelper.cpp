#include "stdafx.h"
#include "TaskbarHelper.h"
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
