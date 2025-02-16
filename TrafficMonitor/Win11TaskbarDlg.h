#pragma once
#include "TaskBarDlg.h"
class CWin11TaskbarDlg :
    public CTaskBarDlg
{
public:

private:
    // 通过 CTaskBarDlg 继承
    void InitTaskbarWnd() override;
    virtual void AdjustTaskbarWndPos(bool force_adjust) override;
    void ResetTaskbarPos() override;
    virtual HWND GetParentHwnd() override;

    HWND m_hNotify;     //任务栏通知区域的句柄
    HWND m_hStart;      //开始按钮的句柄
    CRect m_rcNotify;   //任务栏通知区域的矩形区域
    CRect m_rcStart;     //开始按钮的矩形区域
    int m_last_notify_width{};
    int m_last_start_pos{};

    // 通过 CTaskBarDlg 继承
    void CheckTaskbarOnTopOrBottom() override;

};

