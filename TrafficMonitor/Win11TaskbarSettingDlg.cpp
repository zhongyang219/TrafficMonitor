// Win11TaskbarSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "afxdialogex.h"
#include "Win11TaskbarSettingDlg.h"
#include "TaskBarDlg.h"
#include "WindowsSettingHelper.h"


// CWin11TaskbarSettingDlg 对话框

IMPLEMENT_DYNAMIC(CWin11TaskbarSettingDlg, CDialog)

CWin11TaskbarSettingDlg::CWin11TaskbarSettingDlg(TaskBarSettingData& data, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_WIN11_TASKBAR_SETTING_DLG, pParent)
    , m_data(data)
{

}

CWin11TaskbarSettingDlg::~CWin11TaskbarSettingDlg()
{
}

void CWin11TaskbarSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_WINDOW_OFFSET_TOP_EDIT, m_window_offset_top_edit);
    DDX_Control(pDX, IDC_WINDOW_OFFSET_LEFT_EDIT, m_window_offset_left_edit);
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WIDTET_WIDTH_EDIT, m_widgets_width_edit);
}

void CWin11TaskbarSettingDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}


BEGIN_MESSAGE_MAP(CWin11TaskbarSettingDlg, CDialog)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CWin11TaskbarSettingDlg::OnBnClickedRestoreDefaultButton)
END_MESSAGE_MAP()


// CWin11TaskbarSettingDlg 消息处理程序


BOOL CWin11TaskbarSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    EnableDlgCtrl(IDC_TASKBAR_WND_SNAP_CHECK, CTaskBarDlg::IsTaskbarCloseToIconEnable(m_data.tbar_wnd_on_left));
    CheckDlgButton(IDC_TASKBAR_WND_SNAP_CHECK, m_data.tbar_wnd_snap);
    m_window_offset_top_edit.SetRange(-5, 20);
    m_window_offset_top_edit.SetValue(m_data.window_offset_top);
    m_window_offset_left_edit.SetRange(-800, 800);
    m_window_offset_left_edit.SetValue(m_data.window_offset_left);
    CheckDlgButton(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, m_data.avoid_overlap_with_widgets);
    EnableDlgCtrl(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());
    m_widgets_width_edit.SetRange(0, 300);
    m_widgets_width_edit.SetValue(m_data.taskbar_left_space_win11);
    m_widgets_width_edit.EnableWindow(CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CWin11TaskbarSettingDlg::OnOK()
{
    m_data.tbar_wnd_snap = (IsDlgButtonChecked(IDC_TASKBAR_WND_SNAP_CHECK) != 0);

    m_data.window_offset_top = m_window_offset_top_edit.GetValue();
    m_data.ValidWindowOffsetTop();
    m_data.window_offset_left = m_window_offset_left_edit.GetValue();
    m_data.ValidWindowOffsetLeft();

    m_data.avoid_overlap_with_widgets = (IsDlgButtonChecked(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK) != 0);

    m_data.taskbar_left_space_win11 = m_widgets_width_edit.GetValue();
    if (m_data.taskbar_left_space_win11 < 0)
        m_data.taskbar_left_space_win11 = 0;
    if (m_data.taskbar_left_space_win11 > 300)
        m_data.taskbar_left_space_win11 = 300;

    CDialog::OnOK();
}


void CWin11TaskbarSettingDlg::OnBnClickedRestoreDefaultButton()
{
    m_window_offset_top_edit.SetValue(0);
    m_window_offset_left_edit.SetValue(0);
    m_widgets_width_edit.SetValue(160);
}
