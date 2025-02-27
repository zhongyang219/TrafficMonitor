#pragma once
#include "TaskBarDlg.h"
class CClassicalTaskbarDlg :
    public CTaskBarDlg
{
public:

private:
    // 通过 CTaskBarDlg 继承
    virtual void AdjustTaskbarWndPos(bool force_adjust) override;
    void InitTaskbarWnd() override;
    void ResetTaskbarPos() override;
    virtual HWND GetParentHwnd() override;

private:
    CRect m_rcMinOri;   //初始状态时最小化窗口的矩形区域
    int m_left_space{};			//最小化窗口和二级窗口窗口左侧的边距
    int m_top_space{};			//最小化窗口和二级窗口窗口顶部的边距（用于任务栏在屏幕左侧或右侧时）
    HWND m_hBar;		//任务栏窗口二级容器的句柄
    HWND m_hMin;		//最小化窗口的句柄
    CRect m_rcBar;		//初始状态时任务栏窗口的矩形区域
    CRect m_rcMin;		//最小化窗口的矩形区域

    int m_last_width;	//用于检测宽度变化的上一次的宽度
    int m_last_height;	//用于检测高度变化的上一次的高度（用于任务栏在屏幕左侧或右侧时）

    // 通过 CTaskBarDlg 继承
    void CheckTaskbarOnTopOrBottom() override;
};

