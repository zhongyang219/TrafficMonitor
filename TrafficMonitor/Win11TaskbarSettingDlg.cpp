// Win11TaskbarSettingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "afxdialogex.h"
#include "Win11TaskbarSettingDlg.h"
#include "TaskBarDlg.h"
#include "Win11TaskbarDlg.h"
#include "WindowsSettingHelper.h"


// CWin11TaskbarSettingDlg 对话框

IMPLEMENT_DYNAMIC(CWin11TaskbarSettingDlg, CBaseDialog)

CWin11TaskbarSettingDlg::CWin11TaskbarSettingDlg(TaskBarSettingData& data, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_WIN11_TASKBAR_SETTING_DLG, pParent)
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

    EnableDlgCtrl(IDC_TASKBAR_WND_SNAP_CHECK, CTaskBarDlg::IsTaskbarCloseToIconEnable(m_data.tbar_wnd_on_left));
    CheckDlgButton(IDC_TASKBAR_WND_SNAP_CHECK, m_data.tbar_wnd_snap);
    m_window_offset_top_edit.SetRange(-20, 20);
    m_window_offset_top_edit.SetValue(m_data.window_offset_top);
    m_window_offset_left_edit.SetRange(-800, 800);
    m_window_offset_left_edit.SetValue(m_data.window_offset_left);
    CheckDlgButton(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, m_data.avoid_overlap_with_widgets);
    CheckDlgButton(IDC_RESERVE_TASKBAR_SPACE_CHECK, m_data.reserve_taskbar_space);

    //竖向任务栏上没法预留空间，把开关置灰，而不是留一个勾了也没反应的选项。
    //这里不改m_data，用户原来的设置要原样保留：任务栏摆回上下时它应当继续生效。
    //Space cannot be reserved on a vertical taskbar, so grey the option out rather than leaving
    //a checkbox that silently does nothing. m_data is deliberately left alone so the user's
    //setting survives untouched and takes effect again once the taskbar goes back to top/bottom.
    const bool taskbar_vertical = CWin11TaskbarDlg::IsTaskbarVertical();
    EnableDlgCtrl(IDC_RESERVE_TASKBAR_SPACE_CHECK, !taskbar_vertical);
    if (taskbar_vertical)
    {
        //置灰的控件收不到鼠标消息，提示框永远弹不出来，所以把原因直接接在标签后面
        //A disabled control receives no mouse messages, so a tooltip would never show on it -
        //append the reason to the label instead.
        CString text;
        GetDlgItemText(IDC_RESERVE_TASKBAR_SPACE_CHECK, text);
        text.AppendFormat(_T(" %s"), CCommon::LoadText(IDS_RESERVE_TASKBAR_SPACE_VERTICAL).GetString());
        SetDlgItemText(IDC_RESERVE_TASKBAR_SPACE_CHECK, text);
    }

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_RESERVE_TASKBAR_SPACE_CHECK),
        CCommon::LoadText(IDS_RESERVE_TASKBAR_SPACE_TIP));
    //EnableDlgCtrl(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK, CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());
    m_widgets_width_edit.SetRange(0, 300);
    m_widgets_width_edit.SetValue(m_data.taskbar_left_space_win11);
    //m_widgets_width_edit.EnableWindow(CWindowsSettingHelper::IsTaskbarWidgetsBtnShown());

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}


BOOL CWin11TaskbarSettingDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CBaseDialog::PreTranslateMessage(pMsg);
}


void CWin11TaskbarSettingDlg::OnOK()
{
    m_data.tbar_wnd_snap = (IsDlgButtonChecked(IDC_TASKBAR_WND_SNAP_CHECK) != 0);

    m_data.window_offset_top = m_window_offset_top_edit.GetValue();
    m_data.ValidWindowOffsetTop();
    m_data.window_offset_left = m_window_offset_left_edit.GetValue();
    m_data.ValidWindowOffsetLeft();

    m_data.avoid_overlap_with_widgets = (IsDlgButtonChecked(IDC_AVOID_OVERLAP_RIGHT_WIDGETS_CHECK) != 0);
    m_data.reserve_taskbar_space = (IsDlgButtonChecked(IDC_RESERVE_TASKBAR_SPACE_CHECK) != 0);

    m_data.taskbar_left_space_win11 = m_widgets_width_edit.GetValue();
    if (m_data.taskbar_left_space_win11 < 0)
        m_data.taskbar_left_space_win11 = 0;
    if (m_data.taskbar_left_space_win11 > 300)
        m_data.taskbar_left_space_win11 = 300;

    CBaseDialog::OnOK();
}


void CWin11TaskbarSettingDlg::OnBnClickedRestoreDefaultButton()
{
    m_window_offset_top_edit.SetValue(0);
    m_window_offset_left_edit.SetValue(0);
    m_widgets_width_edit.SetValue(160);
}
