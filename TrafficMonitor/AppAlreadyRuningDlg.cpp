// AppAlreadyRuningDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "AppAlreadyRuningDlg.h"
#include "afxdialogex.h"


// CAppAlreadyRuningDlg 对话框

IMPLEMENT_DYNAMIC(CAppAlreadyRuningDlg, CBaseDialog)

CAppAlreadyRuningDlg::CAppAlreadyRuningDlg(HWND handel, CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_APP_ALREAD_RUNING_DIALOG, pParent), m_handle(handel)
{

}

CAppAlreadyRuningDlg::~CAppAlreadyRuningDlg()
{
}

void CAppAlreadyRuningDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
}

CString CAppAlreadyRuningDlg::GetDialogName() const
{
    return _T("AppAlreadyRuningDlg");
}


BEGIN_MESSAGE_MAP(CAppAlreadyRuningDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_EXIT_INST_BUTTON, &CAppAlreadyRuningDlg::OnBnClickedExitInstButton)
    ON_BN_CLICKED(IDC_OPEN_SETTINGS_BUTTON, &CAppAlreadyRuningDlg::OnBnClickedOpenSettingsButton)
    ON_BN_CLICKED(IDC_SHOW_HIDE_MAIN_WINDOW_BUTTON, &CAppAlreadyRuningDlg::OnBnClickedShowHideMainWindowButton)
    ON_BN_CLICKED(IDC_SHOW_HIDE_TASKBAR_WINDOW_BUTTON, &CAppAlreadyRuningDlg::OnBnClickedShowHideTaskbarWindowButton)
END_MESSAGE_MAP()


// CAppAlreadyRuningDlg 消息处理程序


BOOL CAppAlreadyRuningDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CAppAlreadyRuningDlg::OnBnClickedExitInstButton()
{
    ::PostMessage(m_handle, WM_COMMAND, ID_APP_EXIT, 0);
}


void CAppAlreadyRuningDlg::OnBnClickedOpenSettingsButton()
{
    ::PostMessage(m_handle, WM_COMMAND, ID_OPTIONS, 0);
}


void CAppAlreadyRuningDlg::OnBnClickedShowHideMainWindowButton()
{
    ::PostMessage(m_handle, WM_COMMAND, ID_SHOW_MAIN_WND, 0);
}


void CAppAlreadyRuningDlg::OnBnClickedShowHideTaskbarWindowButton()
{
    ::PostMessage(m_handle, WM_COMMAND, ID_SHOW_TASK_BAR_WND, 0);
}
