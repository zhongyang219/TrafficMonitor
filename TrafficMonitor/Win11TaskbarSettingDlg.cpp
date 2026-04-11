// Win11TaskbarSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "afxdialogex.h"
#include "Win11TaskbarSettingDlg.h"
#include "TaskBarDlg.h"
#include "WindowsSettingHelper.h"


// CWin11TaskbarSettingDlg 对话框

IMPLEMENT_DYNAMIC(CWin11TaskbarSettingDlg, CBaseDialog)

CWin11TaskbarSettingDlg::CWin11TaskbarSettingDlg(TaskBarSettingData& data, int display_index, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_WIN11_TASKBAR_SETTING_DLG, pParent)
    , m_data(data)
    , m_display_index(display_index)
{

}

CWin11TaskbarSettingDlg::~CWin11TaskbarSettingDlg()
{
}

void CWin11TaskbarSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_WINDOW_OFFSET_TOP_EDIT, m_window_offset_top_edit);
    DDX_Control(pDX, IDC_WINDOW_OFFSET_LEFT_EDIT, m_window_offset_left_edit);
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WIDTET_WIDTH_EDIT, m_widgets_width_edit);
}

CString CWin11TaskbarSettingDlg::GetDialogName() const
{
    return _T("Win11TaskbarSettingDlg");
}

bool CWin11TaskbarSettingDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_VERTICAL_OFFSET_STATIC },
        { CtrlTextInfo::L3, IDC_WINDOW_OFFSET_TOP_EDIT },
        { CtrlTextInfo::L2, IDC_PIXEL_STATIC },
        { CtrlTextInfo::L4, IDC_HORIZONTAL_OFFSET_STATIC },
        { CtrlTextInfo::L3, IDC_WINDOW_OFFSET_LEFT_EDIT },
        { CtrlTextInfo::L2, IDC_PIXEL_STATIC1 },
        });
    RepositionTextBasedControls({
        { CtrlTextInfo::L4, IDC_WIDGET_WIDTH_STATIC },
        { CtrlTextInfo::L3, IDC_WIDTET_WIDTH_EDIT },
        { CtrlTextInfo::L2, IDC_PIXEL_STATIC2 },
        });

    return true;
}

void CWin11TaskbarSettingDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}


BEGIN_MESSAGE_MAP(CWin11TaskbarSettingDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CWin11TaskbarSettingDlg::OnBnClickedRestoreDefaultButton)
END_MESSAGE_MAP()


// CWin11TaskbarSettingDlg 消息处理程序


BOOL CWin11TaskbarSettingDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();
    SetIcon(theApp.GetMenuIcon(IDI_TASKBAR_WINDOW), FALSE);

    CString title;
    GetWindowText(title);
    CString display_name;
    if (m_display_index <= 0)
        display_name = CCommon::LoadText(IDS_PRIMARY_DISPLAY);
    else
        display_name = CCommon::LoadTextFormat(IDS_SECONDARY_DISPLAY, { m_display_index });
    title += _T(" - ");
    title += display_name;
    SetWindowText(title);

    EnableDlgCtrl(IDC_TASKBAR_WND_SNAP_CHECK, CTaskBarDlg::IsTaskbarCloseToIconEnable(m_data.tbar_wnd_on_left));
    CheckDlgButton(IDC_TASKBAR_WND_SNAP_CHECK, m_data.IsTaskbarWndSnapForDisplay(m_display_index));
    m_window_offset_top_edit.SetRange(-20, 20);
    m_window_offset_top_edit.SetValue(m_data.GetWindowOffsetTopForDisplay(m_display_index));
    m_window_offset_left_edit.SetRange(-800, 800);
    m_window_offset_left_edit.SetValue(m_data.GetWindowOffsetLeftForDisplay(m_display_index));
    CheckDlgButton(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, m_data.IsAvoidOverlapWithWidgetsForDisplay(m_display_index));
    //EnableDlgCtrl(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());
    m_widgets_width_edit.SetRange(0, 300);
    m_widgets_width_edit.SetValue(m_data.GetTaskbarLeftSpaceForDisplay(m_display_index));
    //m_widgets_width_edit.EnableWindow(CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


void CWin11TaskbarSettingDlg::OnOK()
{
    m_data.SetTaskbarWndSnapForDisplay(m_display_index, (IsDlgButtonChecked(IDC_TASKBAR_WND_SNAP_CHECK) != 0));

    m_data.SetWindowOffsetTopForDisplay(m_display_index, m_window_offset_top_edit.GetValue());
    m_data.SetWindowOffsetLeftForDisplay(m_display_index, m_window_offset_left_edit.GetValue());

    m_data.SetAvoidOverlapWithWidgetsForDisplay(m_display_index, (IsDlgButtonChecked(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK) != 0));
    m_data.SetTaskbarLeftSpaceForDisplay(m_display_index, m_widgets_width_edit.GetValue());

    CBaseDialog::OnOK();
}


void CWin11TaskbarSettingDlg::OnBnClickedRestoreDefaultButton()
{
    m_window_offset_top_edit.SetValue(0);
    m_window_offset_left_edit.SetValue(0);
    m_widgets_width_edit.SetValue(160);
}
