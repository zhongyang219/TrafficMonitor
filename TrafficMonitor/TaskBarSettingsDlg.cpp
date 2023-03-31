// TaskBarSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarSettingsDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"
#include "CAutoAdaptSettingsDlg.h"
#include "DisplayTextSettingDlg.h"
#include "SetItemOrderDlg.h"
#include "WindowsSettingHelper.h"
#include "TrafficMonitorDlg.h"

// CTaskBarSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CTaskBarSettingsDlg, CTabDlg)

CTaskBarSettingsDlg::CTaskBarSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_TASKBAR_SETTINGS_DIALOG, pParent)
{

}

CTaskBarSettingsDlg::~CTaskBarSettingsDlg()
{
}

bool CTaskBarSettingsDlg::IsStyleModified()
{
    bool modified{};
    modified |= (theApp.m_taskbar_data.text_colors != m_data.text_colors);
    modified |= (theApp.m_taskbar_data.back_color != m_data.back_color);
    modified |= (theApp.m_taskbar_data.transparent_color != m_data.transparent_color);
    modified |= (theApp.m_taskbar_data.status_bar_color != m_data.status_bar_color);
    modified |= (theApp.m_taskbar_data.specify_each_item_color != m_data.specify_each_item_color);
    return modified && m_style_modified;
}

void CTaskBarSettingsDlg::DrawStaticColor()
{
    //CCommon::FillStaticColor(m_text_color_static, m_data.text_color);
    //CCommon::FillStaticColor(m_back_color_static, m_data.back_color);
    if (m_data.specify_each_item_color)
    {
        int color_num{};
#ifdef WITHOUT_TEMPERATURE
        color_num = 8;
#else
        color_num = 16;
#endif
        int i{};
        m_text_color_static.SetColorNum(color_num);
        for (const auto& item : m_data.text_colors)
        {
            m_text_color_static.SetFillColor(i, item.second.label);
            m_text_color_static.SetFillColor(i + 1, item.second.value);
            i += 2;
        }
        m_text_color_static.Invalidate();
    }
    else if (!m_data.text_colors.empty())
    {
        m_text_color_static.SetFillColor(m_data.text_colors.begin()->second.label);
    }
    m_back_color_static.SetFillColor(m_data.back_color);
    //m_trans_color_static.SetFillColor(m_data.transparent_color);
    m_status_bar_color_static.SetFillColor(m_data.status_bar_color);
}

void CTaskBarSettingsDlg::IniUnitCombo()
{
    m_unit_combo.ResetContent();
    m_unit_combo.AddString(CCommon::LoadText(IDS_AUTO));
    if (m_data.unit_byte)
    {
        m_unit_combo.AddString(CCommon::LoadText(IDS_FIXED_AS, _T(" KB/s")));
        m_unit_combo.AddString(CCommon::LoadText(IDS_FIXED_AS, _T(" MB/s")));
    }
    else
    {
        m_unit_combo.AddString(CCommon::LoadText(IDS_FIXED_AS, _T(" Kb/s")));
        m_unit_combo.AddString(CCommon::LoadText(IDS_FIXED_AS, _T(" Mb/s")));
    }
    m_unit_combo.SetCurSel(static_cast<int>(m_data.speed_unit));
}

void CTaskBarSettingsDlg::ApplyDefaultStyle(int index)
{
    theApp.m_taskbar_default_style.ApplyDefaultStyle(index, m_data);
    DrawStaticColor();
    ((CButton*)GetDlgItem(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK))->SetCheck(m_data.specify_each_item_color);
    m_background_transparent_chk.SetCheck(m_data.IsTaskbarTransparent());
}

void CTaskBarSettingsDlg::ModifyDefaultStyle(int index)
{
    theApp.m_taskbar_default_style.ModifyDefaultStyle(index, m_data);
}

void CTaskBarSettingsDlg::EnableControl()
{
    bool exe_path_enable = (m_data.double_click_action == DoubleClickAction::SEPCIFIC_APP);
    ShowDlgCtrl(IDC_EXE_PATH_STATIC, exe_path_enable);
    ShowDlgCtrl(IDC_EXE_PATH_EDIT, exe_path_enable);
    ShowDlgCtrl(IDC_BROWSE_BUTTON, exe_path_enable);
    EnableDlgCtrl(IDC_AUTO_ADAPT_SETTINGS_BUTTON, m_data.auto_adapt_light_theme);
    EnableDlgCtrl(IDC_SHOW_DASHED_BOX, m_data.show_status_bar || m_data.show_netspeed_figure);
    m_status_bar_color_static.EnableWindow(m_data.show_status_bar || m_data.show_netspeed_figure);
    EnableDlgCtrl(IDC_CM_GRAPH_BAR_RADIO, m_data.show_status_bar || m_data.show_netspeed_figure);
    EnableDlgCtrl(IDC_CM_GRAPH_PLOT_RADIO, m_data.show_status_bar || m_data.show_netspeed_figure);
    EnableDlgCtrl(IDC_NET_SPEED_FIGURE_MAX_VALUE_EDIT, m_data.show_netspeed_figure);
    EnableDlgCtrl(IDC_NET_SPEED_FIGURE_MAX_VALUE_UNIT_COMBO, m_data.show_netspeed_figure);
    //EnableDlgCtrl(IDC_TASKBAR_WND_SNAP_CHECK, theApp.m_win_version.IsWindows11OrLater() && !m_data.tbar_wnd_on_left);
}


void CTaskBarSettingsDlg::SetControlMouseWheelEnable(bool enable)
{
    m_unit_combo.SetMouseWheelEnable(enable);
    m_double_click_combo.SetMouseWheelEnable(enable);
    m_digit_number_combo.SetMouseWheelEnable(enable);
    m_font_size_edit.SetMouseWheelEnable(enable);
    m_memory_display_combo.SetMouseWheelEnable(enable);
    m_item_space_edit.SetMouseWheelEnable(enable);
    m_window_offset_top_edit.SetMouseWheelEnable(enable);
    m_vertical_margin_edit.SetMouseWheelEnable(enable);
    m_net_speed_figure_max_val_edit.SetMouseWheelEnable(enable);
    m_net_speed_figure_max_val_unit_combo.SetMouseWheelEnable(enable);
}

void CTaskBarSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_TEXT_COLOR_STATIC1, m_text_color_static);
    DDX_Control(pDX, IDC_TEXT_COLOR_STATIC2, m_back_color_static);
    DDX_Control(pDX, IDC_TEXT_COLOR_STATIC3, m_status_bar_color_static);
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_UNIT_COMBO, m_unit_combo);
    DDX_Control(pDX, IDC_HIDE_UNIT_CHECK, m_hide_unit_chk);
    DDX_Control(pDX, IDC_FONT_SIZE_EDIT1, m_font_size_edit);
    DDX_Control(pDX, IDC_DOUBLE_CLICK_COMBO, m_double_click_combo);
    DDX_Control(pDX, IDC_DIGIT_NUMBER_COMBO, m_digit_number_combo);
    //DDX_Control(pDX, IDC_TRANSPARENT_COLOR_STATIC, m_trans_color_static);
    DDX_Control(pDX, IDC_BACKGROUND_TRANSPARENT_CHECK, m_background_transparent_chk);
    DDX_Control(pDX, IDC_AUTO_ADAPT_LIGHT_THEME_CHECK, m_atuo_adapt_light_theme_chk);
    DDX_Control(pDX, IDC_AUTO_SET_BACK_COLOR_CHECK, m_auto_set_back_color_chk);
    DDX_Control(pDX, IDC_MEMORY_DISPLAY_COMBO, m_memory_display_combo);
    DDX_Control(pDX, IDC_ITEM_SPACE_EDIT, m_item_space_edit);
    DDX_Control(pDX, IDC_WINDOW_OFFSET_TOP_EDIT, m_window_offset_top_edit);
    DDX_Control(pDX, IDC_VERTICAL_MARGIN_EDIT, m_vertical_margin_edit);
    DDX_Control(pDX, IDC_NET_SPEED_FIGURE_MAX_VALUE_EDIT, m_net_speed_figure_max_val_edit);
    DDX_Control(pDX, IDC_NET_SPEED_FIGURE_MAX_VALUE_UNIT_COMBO, m_net_speed_figure_max_val_unit_combo);
}


BEGIN_MESSAGE_MAP(CTaskBarSettingsDlg, CTabDlg)
    ON_BN_CLICKED(IDC_SET_FONT_BUTTON1, &CTaskBarSettingsDlg::OnBnClickedSetFontButton1)
    ON_BN_CLICKED(IDC_TASKBAR_WND_ON_LEFT_CHECK, &CTaskBarSettingsDlg::OnBnClickedTaskbarWndOnLeftCheck)
    ON_BN_CLICKED(IDC_SPEED_SHORT_MODE_CHECK, &CTaskBarSettingsDlg::OnBnClickedSpeedShortModeCheck)
    ON_CBN_SELCHANGE(IDC_UNIT_COMBO, &CTaskBarSettingsDlg::OnCbnSelchangeUnitCombo)
    ON_BN_CLICKED(IDC_HIDE_UNIT_CHECK, &CTaskBarSettingsDlg::OnBnClickedHideUnitCheck)
    ON_BN_CLICKED(IDC_VALUE_RIGHT_ALIGN_CHECK, &CTaskBarSettingsDlg::OnBnClickedValueRightAlignCheck)
    ON_BN_CLICKED(IDC_HIDE_PERCENTAGE_CHECK, &CTaskBarSettingsDlg::OnBnClickedHidePercentageCheck)
    ON_MESSAGE(WM_STATIC_CLICKED, &CTaskBarSettingsDlg::OnStaticClicked)
    ON_BN_CLICKED(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK, &CTaskBarSettingsDlg::OnBnClickedSpecifyEachItemColorCheck)
    ON_CBN_SELCHANGE(IDC_DOUBLE_CLICK_COMBO, &CTaskBarSettingsDlg::OnCbnSelchangeDoubleClickCombo)
    ON_BN_CLICKED(IDC_HORIZONTAL_ARRANGE_CHECK, &CTaskBarSettingsDlg::OnBnClickedHorizontalArrangeCheck)
    ON_BN_CLICKED(IDC_SHOW_STATUS_BAR_CHECK, &CTaskBarSettingsDlg::OnBnClickedShowStatusBarCheck)
    ON_BN_CLICKED(IDC_SEPARATE_VALUE_UNIT_CHECK, &CTaskBarSettingsDlg::OnBnClickedSeparateValueUnitCheck)
    ON_BN_CLICKED(IDC_UNIT_BYTE_RADIO, &CTaskBarSettingsDlg::OnBnClickedUnitByteRadio)
    ON_BN_CLICKED(IDC_UNIT_BIT_RADIO, &CTaskBarSettingsDlg::OnBnClickedUnitBitRadio)
    ON_BN_CLICKED(IDC_SHOW_TOOL_TIP_CHK, &CTaskBarSettingsDlg::OnBnClickedShowToolTipChk)
    ON_BN_CLICKED(IDC_DEFAULT_STYLE_BUTTON, &CTaskBarSettingsDlg::OnBnClickedDefaultStyleButton)
    ON_WM_DESTROY()
    ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CTaskBarSettingsDlg::OnBnClickedBrowseButton)
    ON_BN_CLICKED(IDC_CM_GRAPH_BAR_RADIO, &CTaskBarSettingsDlg::OnBnClickedCMGraphBarRadio)
    ON_BN_CLICKED(IDC_CM_GRAPH_PLOT_RADIO, &CTaskBarSettingsDlg::OnBnClickedCMGraphPLOTRadio)
    ON_BN_CLICKED(IDC_BACKGROUND_TRANSPARENT_CHECK, &CTaskBarSettingsDlg::OnBnClickedBackgroundTransparentCheck)
    ON_BN_CLICKED(IDC_AUTO_ADAPT_SETTINGS_BUTTON, &CTaskBarSettingsDlg::OnBnClickedAutoAdaptSettingsButton)
    ON_BN_CLICKED(IDC_AUTO_ADAPT_LIGHT_THEME_CHECK, &CTaskBarSettingsDlg::OnBnClickedAutoAdaptLightThemeCheck)
    ON_BN_CLICKED(IDC_AUTO_SET_BACK_COLOR_CHECK, &CTaskBarSettingsDlg::OnBnClickedAutoSetBackColorCheck)
    ON_BN_CLICKED(IDC_DISPLAY_TEXT_SETTING_BUTTON, &CTaskBarSettingsDlg::OnBnClickedDisplayTextSettingButton)
    ON_CBN_SELCHANGE(IDC_MEMORY_DISPLAY_COMBO, &CTaskBarSettingsDlg::OnCbnSelchangeMemoryDisplayCombo)
    ON_BN_CLICKED(IDC_SHOW_DASHED_BOX, &CTaskBarSettingsDlg::OnBnClickedShowDashedBox)
    ON_BN_CLICKED(IDC_SET_ORDER_BUTTON, &CTaskBarSettingsDlg::OnBnClickedSetOrderButton)
    ON_BN_CLICKED(IDC_TASKBAR_WND_SNAP_CHECK, &CTaskBarSettingsDlg::OnBnClickedTaskbarWndSnapCheck)
    ON_EN_CHANGE(IDC_ITEM_SPACE_EDIT, &CTaskBarSettingsDlg::OnEnChangeItemSpaceEdit)
    ON_EN_CHANGE(IDC_WINDOW_OFFSET_TOP_EDIT, &CTaskBarSettingsDlg::OnEnChangeWindowOffsetTopEdit)
    ON_EN_CHANGE(IDC_VERTICAL_MARGIN_EDIT, &CTaskBarSettingsDlg::OnEnChangeVerticalMarginEdit)
    ON_BN_CLICKED(IDC_SHOW_NET_SPEED_FIGURE_CHECK, &CTaskBarSettingsDlg::OnBnClickedShowNetSpeedFigureCheck)
    ON_CBN_SELCHANGE(IDC_NET_SPEED_FIGURE_MAX_VALUE_UNIT_COMBO, &CTaskBarSettingsDlg::OnCbnSelchangeNetSpeedFigureMaxValueUnitCombo)
    ON_EN_CHANGE(IDC_NET_SPEED_FIGURE_MAX_VALUE_EDIT, &CTaskBarSettingsDlg::OnEnChangeNetSpeedFigureMaxValueEdit)
    ON_BN_CLICKED(IDC_GDI_RADIO, &CTaskBarSettingsDlg::OnBnClickedGdiRadio)
    ON_BN_CLICKED(IDC_D2D_RADIO, &CTaskBarSettingsDlg::OnBnClickedD2dRadio)
END_MESSAGE_MAP()


// CTaskBarSettingsDlg 消息处理程序


BOOL CTaskBarSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    theApp.m_taskbar_default_style.LoadConfig();

    //初始化各控件状态
    SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font.name);
    //wchar_t buff[16];
    //swprintf_s(buff, L"%d", m_data.font_size);
    //SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
    m_font_size_edit.SetRange(5, 72);
    m_font_size_edit.SetValue(m_data.font.size);

    //SetDlgItemText(IDC_UPLOAD_EDIT1, m_data.disp_str.Get(TDI_UP).c_str());
    //SetDlgItemText(IDC_DOWNLOAD_EDIT1, m_data.disp_str.Get(TDI_DOWN).c_str());
    //SetDlgItemText(IDC_CPU_EDIT1, m_data.disp_str.Get(TDI_CPU).c_str());
    //SetDlgItemText(IDC_MEMORY_EDIT1, m_data.disp_str.Get(TDI_MEMORY).c_str());

    //((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK1))->SetCheck(m_data.swap_up_down);
    ((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->SetCheck(m_data.tbar_wnd_on_left);
    ((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK))->SetCheck(m_data.speed_short_mode);
    ((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->SetCheck(m_data.value_right_align);
    ((CButton*)GetDlgItem(IDC_HORIZONTAL_ARRANGE_CHECK))->SetCheck(m_data.horizontal_arrange);
    ((CButton*)GetDlgItem(IDC_SHOW_STATUS_BAR_CHECK))->SetCheck(m_data.show_status_bar);
    ((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->SetCheck(m_data.separate_value_unit_with_space);
    ((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->SetCheck(m_data.show_tool_tip);

    EnableDlgCtrl(IDC_TASKBAR_WND_SNAP_CHECK, theApp.m_win_version.IsWindows11OrLater());
    CheckDlgButton(IDC_TASKBAR_WND_SNAP_CHECK, m_data.tbar_wnd_snap);

    m_text_color_static.SetLinkCursor();
    m_back_color_static.SetLinkCursor();
    //m_trans_color_static.SetLinkCursor();
    m_status_bar_color_static.SetLinkCursor();
    DrawStaticColor();

#ifdef COMPILE_FOR_WINXP
    m_background_transparent_chk.EnableWindow(FALSE);
#endif // COMPILE_FOR_WINXP

    if (theApp.m_win_version.IsWindows7())
        m_background_transparent_chk.EnableWindow(FALSE);

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK), CCommon::LoadText(IDS_SPEED_SHORT_MODE_TIP));
    m_toolTip.AddTool(&m_atuo_adapt_light_theme_chk, CCommon::LoadText(IDS_AUTO_ADAPT_TIP_INFO));
    m_toolTip.AddTool(GetDlgItem(IDC_SHOW_STATUS_BAR_CHECK), CCommon::LoadText(IDS_SHOW_RESOURCE_USAGE_GRAPH_TIP));
    m_toolTip.AddTool(GetDlgItem(IDC_SHOW_NET_SPEED_FIGURE_CHECK), CCommon::LoadText(IDS_SHOW_NET_SPEED_GRAPH_TIP));


    if (m_data.unit_byte)
        ((CButton*)GetDlgItem(IDC_UNIT_BYTE_RADIO))->SetCheck(TRUE);
    else
        ((CButton*)GetDlgItem(IDC_UNIT_BIT_RADIO))->SetCheck(TRUE);

    IniUnitCombo();

    m_hide_unit_chk.SetCheck(m_data.hide_unit);
    if (m_data.speed_unit == SpeedUnit::AUTO)
    {
        m_hide_unit_chk.SetCheck(FALSE);
        m_data.hide_unit = false;
        m_hide_unit_chk.EnableWindow(FALSE);
    }
    ((CButton*)GetDlgItem(IDC_HIDE_PERCENTAGE_CHECK))->SetCheck(m_data.hide_percent);
    ((CButton*)GetDlgItem(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK))->SetCheck(m_data.specify_each_item_color);
    m_background_transparent_chk.SetCheck(m_data.IsTaskbarTransparent());
    m_atuo_adapt_light_theme_chk.SetCheck(m_data.auto_adapt_light_theme);
    m_auto_set_back_color_chk.SetCheck(m_data.auto_set_background_color);
    m_auto_set_back_color_chk.EnableWindow(theApp.m_win_version.IsWindows8OrLater());

    if (theApp.m_win_version.GetMajorVersion() < 10)
    {
        m_data.auto_adapt_light_theme = false;
        m_atuo_adapt_light_theme_chk.EnableWindow(FALSE);
    }

    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_CONNECTION_DETIAL));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_HISTORICAL_TRAFFIC));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_SHOW_HIDE_CPU_MEMORY));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_OPTION_SETTINGS));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_TASK_MANAGER));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_SPECIFIC_APP));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_NONE));
    m_double_click_combo.SetCurSel(static_cast<int>(m_data.double_click_action));

    m_digit_number_combo.AddString(_T("3"));
    m_digit_number_combo.AddString(_T("4"));
    m_digit_number_combo.AddString(_T("5"));
    m_digit_number_combo.AddString(_T("6"));
    m_digit_number_combo.AddString(_T("7"));
    m_digit_number_combo.SetCurSel(m_data.digits_number - 3);

    SetDlgItemText(IDC_EXE_PATH_EDIT, m_data.double_click_exe.c_str());
    EnableControl();

    //m_default_style_menu.LoadMenu(IDR_TASKBAR_STYLE_MENU);

    if (m_data.cm_graph_type)
        CheckDlgButton(IDC_CM_GRAPH_PLOT_RADIO, TRUE);
    else
        CheckDlgButton(IDC_CM_GRAPH_BAR_RADIO, TRUE);
    CheckDlgButton(IDC_SHOW_DASHED_BOX, m_data.show_graph_dashed_box);
    m_item_space_edit.SetRange(0, 32);
    m_item_space_edit.SetValue(m_data.item_space);
    CTaskBarDlg* taskbar_dlg{ CTrafficMonitorDlg::Instance()->GetTaskbarWindow() };
    m_window_offset_top_edit.SetRange(-5, 20);
    m_window_offset_top_edit.SetValue(m_data.window_offset_top);
    if (taskbar_dlg != nullptr)
        m_window_offset_top_edit.EnableWindow(taskbar_dlg->IsTasksbarOnTopOrBottom());
    m_vertical_margin_edit.SetRange(-10, 10);
    m_vertical_margin_edit.SetValue(m_data.vertical_margin);
    if (taskbar_dlg != nullptr)
        m_vertical_margin_edit.EnableWindow(taskbar_dlg->IsTasksbarOnTopOrBottom());

    //初始化内存显示方式下拉列表
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_USAGE_PERCENTAGE));
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_MEMORY_USED));
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_MEMORY_AVAILABLE));
    m_memory_display_combo.SetCurSel(static_cast<int>(m_data.memory_display));

    CheckDlgButton(IDC_SHOW_NET_SPEED_FIGURE_CHECK, m_data.show_netspeed_figure);
    m_net_speed_figure_max_val_edit.SetRange(1, 1024);
    m_net_speed_figure_max_val_edit.SetValue(m_data.netspeed_figure_max_value);
    m_net_speed_figure_max_val_unit_combo.AddString(_T("KB"));
    m_net_speed_figure_max_val_unit_combo.AddString(_T("MB"));
    m_net_speed_figure_max_val_unit_combo.SetCurSel(m_data.netspeed_figure_max_value_unit);

    ((CButton*)GetDlgItem(IDC_SET_ORDER_BUTTON))->SetIcon(theApp.GetMenuIcon(IDI_ITEM));

    //初始化“预设方案”菜单
    m_default_style_menu.CreatePopupMenu();
    m_modify_default_style_menu.CreatePopupMenu();
    for (int i{ 0 }; i < TASKBAR_DEFAULT_STYLE_NUM; i++)
    {
        CString item_name;
        item_name.Format(_T("%s %d"), CCommon::LoadText(IDS_PRESET).GetString(), i + 1);
        m_default_style_menu.AppendMenu(MF_STRING | MF_ENABLED, ID_DEFAULT_STYLE1 + i, item_name);
        m_modify_default_style_menu.AppendMenu(MF_STRING | MF_ENABLED, ID_MODIFY_DEFAULT_STYLE1 + i, item_name);
    }
    m_default_style_menu.AppendMenu(MF_SEPARATOR);
    m_default_style_menu.AppendMenu(MF_POPUP | MF_STRING, (UINT)m_modify_default_style_menu.m_hMenu, CCommon::LoadText(IDS_MODIFY_PRESET));

    //设置是否禁用D2D
    if (!CTaskBarDlgDrawCommonSupport::CheckSupport())
    {
        m_data.disable_d2d = true;
        //不支持时禁用选项
        EnableDlgCtrl(IDC_D2D_RADIO, false);
    }

    if (m_data.disable_d2d)
        CheckDlgButton(IDC_GDI_RADIO, true);
    else
        CheckDlgButton(IDC_D2D_RADIO, true);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarSettingsDlg::OnBnClickedSetFontButton1()
{
    // TODO: 在此添加控件通知处理程序代码
    LOGFONT lf{};
    lf.lfHeight = FontSizeToLfHeight(m_data.font.size);
    lf.lfWeight = (m_data.font.bold ? FW_BOLD : FW_NORMAL);
    lf.lfItalic = m_data.font.italic;
    lf.lfUnderline = m_data.font.underline;
    lf.lfStrikeOut = m_data.font.strike_out;
    lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
    //wcsncpy_s(lf.lfFaceName, m_data.font.name.GetString(), 32);
    CCommon::WStringCopy(lf.lfFaceName, 32, m_data.font.name.GetString());
    CCommon::NormalizeFont(lf);
    CFontDialog fontDlg(&lf);   //构造字体对话框，初始选择字体为之前字体
    if (IDOK == fontDlg.DoModal())     // 显示字体对话框
    {
        //获取字体信息
        m_data.font.name = fontDlg.GetFaceName();
        m_data.font.size = fontDlg.GetSize() / 10;
        m_data.font.bold = (fontDlg.IsBold() != FALSE);
        m_data.font.italic = (fontDlg.IsItalic() != FALSE);
        m_data.font.underline = (fontDlg.IsUnderline() != FALSE);
        m_data.font.strike_out = (fontDlg.IsStrikeOut() != FALSE);
        //将字体信息显示出来
        SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font.name);
        wchar_t buff[16];
        swprintf_s(buff, L"%d", m_data.font.size);
        SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
    }
}


void CTaskBarSettingsDlg::OnBnClickedTaskbarWndOnLeftCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.tbar_wnd_on_left = (((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->GetCheck() != 0);
    EnableControl();
}


void CTaskBarSettingsDlg::OnBnClickedSpeedShortModeCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.speed_short_mode = (((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK))->GetCheck() != 0);
}


BOOL CTaskBarSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CTaskBarSettingsDlg::OnCbnSelchangeUnitCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.speed_unit = static_cast<SpeedUnit>(m_unit_combo.GetCurSel());
    if (m_data.speed_unit == SpeedUnit::AUTO)
    {
        m_hide_unit_chk.SetCheck(FALSE);
        m_data.hide_unit = false;
        m_hide_unit_chk.EnableWindow(FALSE);
    }
    else
    {
        m_hide_unit_chk.EnableWindow(TRUE);
    }
}


void CTaskBarSettingsDlg::OnBnClickedHideUnitCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_unit = (m_hide_unit_chk.GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类
    //获取字体设置
    int font_size;
    font_size = m_font_size_edit.GetValue();
    if (font_size > MAX_FONT_SIZE || font_size < MIN_FONT_SIZE)
    {
        CString info;
        info.Format(CCommon::LoadText(IDS_FONT_SIZE_WARNING), MIN_FONT_SIZE, MAX_FONT_SIZE);
        MessageBox(info, NULL, MB_OK | MB_ICONWARNING);
    }
    else
    {
        m_data.font.size = font_size;
    }
    GetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font.name);

    //获取数据位数的设置
    m_data.digits_number = m_digit_number_combo.GetCurSel() + 3;

    bool is_taskbar_transparent_checked = (m_background_transparent_chk.GetCheck() != 0);
    m_data.SetTaskabrTransparent(is_taskbar_transparent_checked);

    SaveColorSettingToDefaultStyle();

    CTabDlg::OnOK();
}


void CTaskBarSettingsDlg::SaveColorSettingToDefaultStyle()
{
    //如果开启了自动适应Windows10深色/浅色模式功能时，自动将当前配置保存到对应预设
    if (theApp.m_taskbar_data.auto_save_taskbar_color_settings_to_preset && m_data.auto_adapt_light_theme && IsStyleModified())
    {
        int default_style_saved{ CWindowsSettingHelper::IsWindows10LightTheme() ? m_data.light_default_style : m_data.dark_default_style };
        ModifyDefaultStyle(default_style_saved);
    }
}

void CTaskBarSettingsDlg::OnBnClickedValueRightAlignCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.value_right_align = (((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedHidePercentageCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_percent = (((CButton*)GetDlgItem(IDC_HIDE_PERCENTAGE_CHECK))->GetCheck() != 0);
}


afx_msg LRESULT CTaskBarSettingsDlg::OnStaticClicked(WPARAM wParam, LPARAM lParam)
{
    switch (::GetDlgCtrlID(((CWnd*)wParam)->m_hWnd))
    {
    case IDC_TEXT_COLOR_STATIC1:        //点击“文本颜色”时
    {
        //设置文本颜色
        if (m_data.specify_each_item_color)
        {
            CTaskbarColorDlg colorDlg(m_data.text_colors);
            if (colorDlg.DoModal() == IDOK)
            {
                m_data.text_colors = colorDlg.GetColors();
                DrawStaticColor();
                m_style_modified = true;
            }
        }
        else if (!m_data.text_colors.empty())
        {
            CMFCColorDialogEx colorDlg(m_data.text_colors.begin()->second.label, 0, this);
            if (colorDlg.DoModal() == IDOK)
            {
                m_data.text_colors.begin()->second.label = colorDlg.GetColor();
                if (m_data.back_color == m_data.text_colors.begin()->second.label)
                    MessageBox(CCommon::LoadText(IDS_SAME_TEXT_BACK_COLOR_WARNING), NULL, MB_ICONWARNING);
                DrawStaticColor();
                m_style_modified = true;
            }
        }
        break;
    }
    case IDC_TEXT_COLOR_STATIC2:        //点击“背景颜色”时
    {
        //设置背景颜色
        CMFCColorDialogEx colorDlg(m_data.back_color, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            bool background_transparent = m_data.IsTaskbarTransparent();
            m_data.back_color = colorDlg.GetColor();
            if (m_data.back_color == m_data.text_colors.begin()->second.label)
                MessageBox(CCommon::LoadText(IDS_SAME_BACK_TEXT_COLOR_WARNING), NULL, MB_ICONWARNING);
            if (background_transparent)
            {
                CCommon::TransparentColorConvert(m_data.back_color);
                //如果当前设置了背景透明，则更改了背景色后同时将透明色设置成和背景色一样的颜色，以保持背景透明
                m_data.transparent_color = m_data.back_color;
            }
            DrawStaticColor();
            m_style_modified = true;
        }
        break;
    }
    //case IDC_TRANSPARENT_COLOR_STATIC:        //点击“透明色”时
    //{
    //  CMFCColorDialogEx colorDlg(m_data.transparent_color, 0, this);
    //  if (colorDlg.DoModal() == IDOK)
    //  {
    //      m_data.transparent_color = colorDlg.GetColor();
    //      DrawStaticColor();
    //  }
    //  break;
    //}
    case IDC_TEXT_COLOR_STATIC3:        //点击“状态条颜色”时
    {
        CMFCColorDialogEx colorDlg(m_data.status_bar_color, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            m_data.status_bar_color = colorDlg.GetColor();
            DrawStaticColor();
            m_style_modified = true;
        }
        break;
    }
    default:
        break;
    }
    return 0;
}


void CTaskBarSettingsDlg::OnBnClickedSpecifyEachItemColorCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.specify_each_item_color = (((CButton*)GetDlgItem(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK))->GetCheck() != 0);
    DrawStaticColor();
    m_style_modified = true;
}


void CTaskBarSettingsDlg::OnCbnSelchangeDoubleClickCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.double_click_action = static_cast<DoubleClickAction>(m_double_click_combo.GetCurSel());
    EnableControl();
}


void CTaskBarSettingsDlg::OnBnClickedHorizontalArrangeCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.horizontal_arrange = (((CButton*)GetDlgItem(IDC_HORIZONTAL_ARRANGE_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedShowStatusBarCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_status_bar = (((CButton*)GetDlgItem(IDC_SHOW_STATUS_BAR_CHECK))->GetCheck() != 0);
    EnableControl();
}


void CTaskBarSettingsDlg::OnBnClickedSeparateValueUnitCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.separate_value_unit_with_space = (((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedUnitByteRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.unit_byte = true;
    IniUnitCombo();
}


void CTaskBarSettingsDlg::OnBnClickedUnitBitRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.unit_byte = false;
    IniUnitCombo();
}


void CTaskBarSettingsDlg::OnBnClickedShowToolTipChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_tool_tip = (((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedDefaultStyleButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CWnd* pBtn = GetDlgItem(IDC_DEFAULT_STYLE_BUTTON);
    CPoint point;
    if (pBtn != nullptr)
    {
        CRect rect;
        pBtn->GetWindowRect(rect);
        point.x = rect.left;
        point.y = rect.bottom;
        m_default_style_menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    }
}


void CTaskBarSettingsDlg::OnDestroy()
{
    CTabDlg::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
    theApp.m_taskbar_default_style.SaveConfig();
}


void CTaskBarSettingsDlg::OnBnClickedBrowseButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CString szFilter = CCommon::LoadText(IDS_EXE_FILTER);
    CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
    if (IDOK == fileDlg.DoModal())
    {
        m_data.double_click_exe = fileDlg.GetPathName();
        SetDlgItemText(IDC_EXE_PATH_EDIT, m_data.double_click_exe.c_str());
    }
}

void CTaskBarSettingsDlg::OnBnClickedCMGraphPLOTRadio()
{
    m_data.cm_graph_type = true;
}

void CTaskBarSettingsDlg::OnBnClickedCMGraphBarRadio()
{
    m_data.cm_graph_type = false;
}


void CTaskBarSettingsDlg::OnBnClickedBackgroundTransparentCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    bool checked = (m_background_transparent_chk.GetCheck() != 0);
    m_data.SetTaskabrTransparent(checked);
    m_style_modified = true;
}


void CTaskBarSettingsDlg::OnBnClickedAutoAdaptSettingsButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CAutoAdaptSettingsDlg dlg(m_data);
    dlg.DoModal();
}


void CTaskBarSettingsDlg::OnBnClickedAutoAdaptLightThemeCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_adapt_light_theme = (m_atuo_adapt_light_theme_chk.GetCheck() != 0);
    EnableControl();
}


void CTaskBarSettingsDlg::OnBnClickedAutoSetBackColorCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.auto_set_background_color = (m_auto_set_back_color_chk.GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedDisplayTextSettingButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CDisplayTextSettingDlg dlg(m_data.disp_str);
    dlg.DoModal();
}


void CTaskBarSettingsDlg::OnCbnSelchangeMemoryDisplayCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.memory_display = static_cast<MemoryDisplay>(m_memory_display_combo.GetCurSel());
}


void CTaskBarSettingsDlg::OnBnClickedShowDashedBox()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_graph_dashed_box = (IsDlgButtonChecked(IDC_SHOW_DASHED_BOX) != 0);
}


void CTaskBarSettingsDlg::OnBnClickedSetOrderButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CSetItemOrderDlg dlg;
    dlg.SetItemOrder(m_data.item_order.GetItemOrderConst());
    dlg.SetDisplayItem(m_data.m_tbar_display_item);
    dlg.SetPluginDisplayItem(m_data.plugin_display_item);
    if (dlg.DoModal() == IDOK)
    {
        m_data.item_order.SetOrder(dlg.GetItemOrder());
        m_data.m_tbar_display_item = dlg.GetDisplayItem();
        m_data.plugin_display_item = dlg.GetPluginDisplayItem();
    }
}


void CTaskBarSettingsDlg::OnBnClickedTaskbarWndSnapCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.tbar_wnd_snap = (IsDlgButtonChecked(IDC_TASKBAR_WND_SNAP_CHECK) != 0);
}


void CTaskBarSettingsDlg::OnEnChangeItemSpaceEdit()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CTabDlg::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    m_data.item_space = m_item_space_edit.GetValue();
    m_data.ValidItemSpace();
}

void CTaskBarSettingsDlg::OnEnChangeWindowOffsetTopEdit()
{
    m_data.window_offset_top = m_window_offset_top_edit.GetValue();
    m_data.ValidWindowOffsetTop();
}

void CTaskBarSettingsDlg::OnEnChangeVerticalMarginEdit()
{
    m_data.vertical_margin = m_vertical_margin_edit.GetValue();
    m_data.ValidVerticalMargin();
}

BOOL CTaskBarSettingsDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // TODO: 在此添加专用代码和/或调用基类
    UINT cmd = LOWORD(wParam);

    if (cmd >= ID_DEFAULT_STYLE1 && cmd < ID_DEFAULT_STYLE_MAX)
    {
        int default_style = cmd - ID_DEFAULT_STYLE1;
        ApplyDefaultStyle(default_style);
    }
    if (cmd >= ID_MODIFY_DEFAULT_STYLE1 && cmd < ID_MODIFY_DEFAULT_STYLE_MAX)
    {
        int default_style = cmd - ID_MODIFY_DEFAULT_STYLE1;
        if (MessageBox(CCommon::LoadTextFormat(IDS_SAVE_DEFAULT_STYLE_INQUIRY, { default_style + 1 }), NULL, MB_ICONQUESTION | MB_YESNO) == IDYES)
        {
            ModifyDefaultStyle(default_style);
        }
    }

    return CTabDlg::OnCommand(wParam, lParam);
}


void CTaskBarSettingsDlg::OnBnClickedShowNetSpeedFigureCheck()
{
    m_data.show_netspeed_figure = (IsDlgButtonChecked(IDC_SHOW_NET_SPEED_FIGURE_CHECK) != 0);
    EnableControl();
}


void CTaskBarSettingsDlg::OnCbnSelchangeNetSpeedFigureMaxValueUnitCombo()
{
    m_data.netspeed_figure_max_value_unit = m_net_speed_figure_max_val_unit_combo.GetCurSel();
}


void CTaskBarSettingsDlg::OnEnChangeNetSpeedFigureMaxValueEdit()
{
    m_data.netspeed_figure_max_value = m_net_speed_figure_max_val_edit.GetValue();
}


void CTaskBarSettingsDlg::OnBnClickedGdiRadio()
{
    m_data.disable_d2d = true;
}


void CTaskBarSettingsDlg::OnBnClickedD2dRadio()
{
    m_data.disable_d2d = false;
}
