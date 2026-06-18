#pragma once
#include "TaskBarDlg.h"
class CWineTaskbarDlg :
	public CTaskBarDlg
{
private:
	// 通过 CTaskBarDlg 继承
	void InitTaskbarWnd() override;
	void AdjustTaskbarWndPos(bool force_adjust) override;
	void ResetTaskbarPos() override;
	void CheckTaskbarOnTopOrBottom() override;
	HWND GetParentHwnd() override;
};
