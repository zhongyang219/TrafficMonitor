#pragma once
#include "TaskBarDlg.h"
class CWineTaskbarDlg :
    public CTaskBarDlg
{
private:
    // Í¨¹ý CTaskBarDlg ¼Ì³Ð
    void InitTaskbarWnd() override;
    void AdjustTaskbarWndPos(bool force_adjust) override;
    void ResetTaskbarPos() override;
    void CheckTaskbarOnTopOrBottom() override;
    HWND GetParentHwnd() override;
};

