#pragma once
#include "BaseDialog.h"

// CAppAlreadyRuningDlg 对话框

class CAppAlreadyRuningDlg : public CBaseDialog
{
    DECLARE_DYNAMIC(CAppAlreadyRuningDlg)

public:
    CAppAlreadyRuningDlg(HWND handel, CWnd* pParent = nullptr);   // 标准构造函数
    virtual ~CAppAlreadyRuningDlg();

    // 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_APP_ALREAD_RUNING_DIALOG };
#endif

private:
    HWND m_handle{};        //正在运行的TrafficMonitor进程主窗口的句柄

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
    virtual CString GetDialogName() const override;

    DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedExitInstButton();
    afx_msg void OnBnClickedOpenSettingsButton();
    afx_msg void OnBnClickedShowHideMainWindowButton();
    afx_msg void OnBnClickedShowHideTaskbarWindowButton();
};
