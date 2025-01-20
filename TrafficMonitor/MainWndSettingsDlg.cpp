// MainWndSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "MainWndSettingsDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"
#include "DisplayTextSettingDlg.h"
#include "FileDialogEx.h"

// CMainWndSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CMainWndSettingsDlg, CTabDlg)

CMainWndSettingsDlg::CMainWndSettingsDlg(CWnd* pParent /*=NULL*/)
    : CTabDlg(IDD_MAIN_WND_SETTINGS_DIALOG, pParent)
{

}

CMainWndSettingsDlg::~CMainWndSettingsDlg()
{
}

void CMainWndSettingsDlg::SetControlMouseWheelEnable(bool enable)
{
    m_unit_combo.SetMouseWheelEnable(enable);
    m_double_click_combo.SetMouseWheelEnable(enable);
    m_font_size_edit.SetMouseWheelEnable(enable);
    m_memory_display_combo.SetMouseWheelEnable(enable);
}

void CMainWndSettingsDlg::DrawStaticColor()
{
    //CCommon::FillStaticColor(m_color_static, m_data.text_color);
    if (m_data.text_colors.empty())
        return;
    if (m_data.specify_each_item_color)
    {
        int color_num{};
#ifdef WITHOUT_TEMPERATURE
        color_num = 4;
#else
        color_num = 8;
#endif
        m_color_static.SetColorNum(color_num);
        int index{};
        for (const auto& item : m_data.text_colors)
        {
            m_color_static.SetFillColor(index, item.second);
            index++;
        }
        m_color_static.Invalidate();
    }
    else
    {
        m_color_static.SetFillColor(m_data.text_colors.begin()->second);
    }
}

void CMainWndSettingsDlg::IniUnitCombo()
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

void CMainWndSettingsDlg::EnableControl()
{
    bool exe_path_enable = (m_data.double_click_action == DoubleClickAction::SEPCIFIC_APP);
    CWnd* pWnd{};
    pWnd = GetDlgItem(IDC_EXE_PATH_STATIC);
    if (pWnd != nullptr)
        pWnd->ShowWindow(exe_path_enable ? SW_SHOW : SW_HIDE);
    pWnd = GetDlgItem(IDC_EXE_PATH_EDIT);
    if (pWnd != nullptr)
        pWnd->ShowWindow(exe_path_enable ? SW_SHOW : SW_HIDE);
    pWnd = GetDlgItem(IDC_BROWSE_BUTTON);
    if (pWnd != nullptr)
        pWnd->ShowWindow(exe_path_enable ? SW_SHOW : SW_HIDE);
}

void CMainWndSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_TEXT_COLOR_STATIC, m_color_static);
    CTabDlg::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_HIDE_UNIT_CHECK, m_hide_unit_chk);
    DDX_Control(pDX, IDC_UNIT_COMBO, m_unit_combo);
    DDX_Control(pDX, IDC_FONT_SIZE_EDIT, m_font_size_edit);
    DDX_Control(pDX, IDC_DOUBLE_CLICK_COMBO, m_double_click_combo);
    DDX_Control(pDX, IDC_MEMORY_DISPLAY_COMBO, m_memory_display_combo);
}


BEGIN_MESSAGE_MAP(CMainWndSettingsDlg, CTabDlg)
    //ON_EN_CHANGE(IDC_UPLOAD_EDIT, &CMainWndSettingsDlg::OnEnChangeUploadEdit)
    //ON_EN_CHANGE(IDC_DOWNLOAD_EDIT, &CMainWndSettingsDlg::OnEnChangeDownloadEdit)
    //ON_EN_CHANGE(IDC_CPU_EDIT, &CMainWndSettingsDlg::OnEnChangeCpuEdit)
    //ON_EN_CHANGE(IDC_MEMORY_EDIT, &CMainWndSettingsDlg::OnEnChangeMemoryEdit)
    //ON_BN_CLICKED(IDC_SET_COLOR_BUTTON1, &CMainWndSettingsDlg::OnBnClickedSetColorButton1)
    //ON_BN_CLICKED(IDC_SET_DEFAULT_BUTTON, &CMainWndSettingsDlg::OnBnClickedSetDefaultButton)
    ON_BN_CLICKED(IDC_SET_FONT_BUTTON, &CMainWndSettingsDlg::OnBnClickedSetFontButton)
    ON_BN_CLICKED(IDC_SWITCH_UP_DOWN_CHECK, &CMainWndSettingsDlg::OnBnClickedSwitchUpDownCheck)
    ON_BN_CLICKED(IDC_FULLSCREEN_HIDE_CHECK, &CMainWndSettingsDlg::OnBnClickedFullscreenHideCheck)
    ON_BN_CLICKED(IDC_SPEED_SHORT_MODE_CHECK2, &CMainWndSettingsDlg::OnBnClickedSpeedShortModeCheck2)
    ON_CBN_SELCHANGE(IDC_UNIT_COMBO, &CMainWndSettingsDlg::OnCbnSelchangeUnitCombo)
    ON_BN_CLICKED(IDC_HIDE_UNIT_CHECK, &CMainWndSettingsDlg::OnBnClickedHideUnitCheck)
    ON_BN_CLICKED(IDC_HIDE_PERCENTAGE_CHECK, &CMainWndSettingsDlg::OnBnClickedHidePercentageCheck)
    ON_MESSAGE(WM_STATIC_CLICKED, &CMainWndSettingsDlg::OnStaticClicked)
    ON_BN_CLICKED(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK, &CMainWndSettingsDlg::OnBnClickedSpecifyEachItemColorCheck)
    ON_CBN_SELCHANGE(IDC_DOUBLE_CLICK_COMBO, &CMainWndSettingsDlg::OnCbnSelchangeDoubleClickCombo)
    ON_BN_CLICKED(IDC_SEPARATE_VALUE_UNIT_CHECK, &CMainWndSettingsDlg::OnBnClickedSeparateValueUnitCheck)
    ON_BN_CLICKED(IDC_UNIT_BYTE_RADIO, &CMainWndSettingsDlg::OnBnClickedUnitByteRadio)
    ON_BN_CLICKED(IDC_UNIT_BIT_RADIO, &CMainWndSettingsDlg::OnBnClickedUnitBitRadio)
    ON_BN_CLICKED(IDC_SHOW_TOOL_TIP_CHK, &CMainWndSettingsDlg::OnBnClickedShowToolTipChk)
    ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CMainWndSettingsDlg::OnBnClickedBrowseButton)
    ON_BN_CLICKED(IDC_DISPLAY_TEXT_SETTING_BUTTON, &CMainWndSettingsDlg::OnBnClickedDisplayTextSettingButton)
    ON_CBN_SELCHANGE(IDC_MEMORY_DISPLAY_COMBO, &CMainWndSettingsDlg::OnCbnSelchangeMemoryDisplayCombo)
    ON_BN_CLICKED(IDC_ALWAYS_ON_TOP_CHECK, &CMainWndSettingsDlg::OnBnClickedAlwaysOnTopCheck)
    ON_BN_CLICKED(IDC_MOUSE_PENETRATE_CHECK, &CMainWndSettingsDlg::OnBnClickedMousePenetrateCheck)
    ON_BN_CLICKED(IDC_LOCK_WINDOW_POS_CHECK, &CMainWndSettingsDlg::OnBnClickedLockWindowPosCheck)
    ON_BN_CLICKED(IDC_ALOW_OUT_OF_BORDER_CHECK, &CMainWndSettingsDlg::OnBnClickedAlowOutOfBorderCheck)
END_MESSAGE_MAP()


// CMainWndSettingsDlg 消息处理程序


BOOL CMainWndSettingsDlg::OnInitDialog()
{
    CTabDlg::OnInitDialog();

    // TODO:  在此添加额外的初始化

    //初始化各控件状态
    SetDlgItemText(IDC_FONT_NAME_EDIT, m_data.font.name);
    //wchar_t buff[16];
    //swprintf_s(buff, L"%d", m_data.font_size);
    //SetDlgItemText(IDC_FONT_SIZE_EDIT, buff);
    m_font_size_edit.SetRange(5, 72);
    m_font_size_edit.SetValue(m_data.font.size);

    //SetDlgItemText(IDC_UPLOAD_EDIT, m_data.disp_str.Get(TDI_UP).c_str());
    //SetDlgItemText(IDC_DOWNLOAD_EDIT, m_data.disp_str.Get(TDI_DOWN).c_str());
    //SetDlgItemText(IDC_CPU_EDIT, m_data.disp_str.Get(TDI_CPU).c_str());
    //SetDlgItemText(IDC_MEMORY_EDIT, m_data.disp_str.Get(TDI_MEMORY).c_str());

    ((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK))->SetCheck(m_data.swap_up_down);
    ((CButton*)GetDlgItem(IDC_FULLSCREEN_HIDE_CHECK))->SetCheck(m_data.hide_main_wnd_when_fullscreen);
    ((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK2))->SetCheck(m_data.speed_short_mode);
    ((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->SetCheck(m_data.separate_value_unit_with_space);
    ((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->SetCheck(m_data.show_tool_tip);

    m_color_static.SetLinkCursor();
    DrawStaticColor();

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK2), CCommon::LoadText(IDS_SPEED_SHORT_MODE_TIP));

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

    if (m_text_disable)
    {
        //GetDlgItem(IDC_UPLOAD_EDIT)->EnableWindow(FALSE);
        //GetDlgItem(IDC_DOWNLOAD_EDIT)->EnableWindow(FALSE);
        //GetDlgItem(IDC_CPU_EDIT)->EnableWindow(FALSE);
        //GetDlgItem(IDC_MEMORY_EDIT)->EnableWindow(FALSE);
        EnableDlgCtrl(IDC_DISPLAY_TEXT_SETTING_BUTTON, false);
        m_data.swap_up_down = false;
        ((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK))->SetCheck(FALSE);
        GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK)->EnableWindow(FALSE);
        //GetDlgItem(IDC_SET_DEFAULT_BUTTON)->EnableWindow(FALSE);
    }

    CheckDlgButton(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK, m_data.specify_each_item_color);

    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_CONNECTION_DETIAL));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_HISTORICAL_TRAFFIC));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_SHOW_HIDE_MORE_INFO));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_OPTION_SETTINGS));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_TASK_MANAGER));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_SPECIFIC_APP));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_CHANGE_SKIN));
    m_double_click_combo.AddString(CCommon::LoadText(IDS_NONE));
    m_double_click_combo.SetCurSel(static_cast<int>(m_data.double_click_action));

    SetDlgItemText(IDC_EXE_PATH_EDIT, m_data.double_click_exe.c_str());
    EnableControl();

    //初始化内存显示方式下拉列表
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_USAGE_PERCENTAGE));
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_MEMORY_USED));
    m_memory_display_combo.AddString(CCommon::LoadText(IDS_MEMORY_AVAILABLE));
    m_memory_display_combo.SetCurSel(static_cast<int>(m_data.memory_display));

    CheckDlgButton(IDC_ALWAYS_ON_TOP_CHECK, m_data.m_always_on_top);
    CheckDlgButton(IDC_MOUSE_PENETRATE_CHECK, m_data.m_mouse_penetrate);
    CheckDlgButton(IDC_LOCK_WINDOW_POS_CHECK, m_data.m_lock_window_pos);
    CheckDlgButton(IDC_ALOW_OUT_OF_BORDER_CHECK, m_data.m_alow_out_of_border);

    ////设置控件不响应鼠标滚轮消息
    //m_unit_combo.SetMouseWheelEnable(false);
    //m_double_click_combo.SetMouseWheelEnable(false);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


//void CMainWndSettingsDlg::OnEnChangeUploadEdit()
//{
//  // TODO:  如果该控件是 RICHEDIT 控件，它将不
//  // 发送此通知，除非重写 CTabDlg::OnInitDialog()
//  // 函数并调用 CRichEditCtrl().SetEventMask()，
//  // 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//  // TODO:  在此添加控件通知处理程序代码
//  CString tmp;
//  GetDlgItemText(IDC_UPLOAD_EDIT, tmp);
//  m_data.disp_str.Get(TDI_UP) = tmp;
//}
//
//
//void CMainWndSettingsDlg::OnEnChangeDownloadEdit()
//{
//  // TODO:  如果该控件是 RICHEDIT 控件，它将不
//  // 发送此通知，除非重写 CTabDlg::OnInitDialog()
//  // 函数并调用 CRichEditCtrl().SetEventMask()，
//  // 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//  // TODO:  在此添加控件通知处理程序代码
//  CString tmp;
//  GetDlgItemText(IDC_DOWNLOAD_EDIT, tmp);
//  m_data.disp_str.Get(TDI_DOWN) = tmp;
//}
//
//
//void CMainWndSettingsDlg::OnEnChangeCpuEdit()
//{
//  // TODO:  如果该控件是 RICHEDIT 控件，它将不
//  // 发送此通知，除非重写 CTabDlg::OnInitDialog()
//  // 函数并调用 CRichEditCtrl().SetEventMask()，
//  // 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//  // TODO:  在此添加控件通知处理程序代码
//  CString tmp;
//  GetDlgItemText(IDC_CPU_EDIT, tmp);
//  m_data.disp_str.Get(TDI_CPU) = tmp;
//}
//
//
//void CMainWndSettingsDlg::OnEnChangeMemoryEdit()
//{
//  // TODO:  如果该控件是 RICHEDIT 控件，它将不
//  // 发送此通知，除非重写 CTabDlg::OnInitDialog()
//  // 函数并调用 CRichEditCtrl().SetEventMask()，
//  // 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//  // TODO:  在此添加控件通知处理程序代码
//  CString tmp;
//  GetDlgItemText(IDC_MEMORY_EDIT, tmp);
//  m_data.disp_str.Get(TDI_MEMORY) = tmp;
//}


//void CMainWndSettingsDlg::OnBnClickedSetDefaultButton()
//{
//  // TODO: 在此添加控件通知处理程序代码
//  m_data.disp_str.Get(TDI_UP) = CCommon::LoadText(IDS_UPLOAD_DISP, _T(": "));
//  m_data.disp_str.Get(TDI_DOWN) = CCommon::LoadText(IDS_DOWNLOAD_DISP, _T(": "));
//  m_data.disp_str.Get(TDI_CPU) = L"CPU: ";
//  m_data.disp_str.Get(TDI_MEMORY) = CCommon::LoadText(IDS_MEMORY_DISP, _T(": "));
//  SetDlgItemText(IDC_UPLOAD_EDIT, m_data.disp_str.Get(TDI_UP).c_str());
//  SetDlgItemText(IDC_DOWNLOAD_EDIT, m_data.disp_str.Get(TDI_DOWN).c_str());
//  SetDlgItemText(IDC_CPU_EDIT, m_data.disp_str.Get(TDI_CPU).c_str());
//  SetDlgItemText(IDC_MEMORY_EDIT, m_data.disp_str.Get(TDI_MEMORY).c_str());
//}


void CMainWndSettingsDlg::OnBnClickedSetFontButton()
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
        SetDlgItemText(IDC_FONT_NAME_EDIT, m_data.font.name);
        SetDlgItemText(IDC_FONT_SIZE_EDIT, std::to_wstring(m_data.font.size).c_str());
    }
}


void CMainWndSettingsDlg::OnBnClickedSwitchUpDownCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.swap_up_down = (((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK))->GetCheck() != 0);
}


void CMainWndSettingsDlg::OnBnClickedFullscreenHideCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_main_wnd_when_fullscreen = (((CButton*)GetDlgItem(IDC_FULLSCREEN_HIDE_CHECK))->GetCheck() != 0);
}


void CMainWndSettingsDlg::OnBnClickedSpeedShortModeCheck2()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.speed_short_mode = (((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK2))->GetCheck() != 0);
}


void CMainWndSettingsDlg::OnCbnSelchangeUnitCombo()
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


void CMainWndSettingsDlg::OnBnClickedHideUnitCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_unit = (m_hide_unit_chk.GetCheck() != 0);
}


BOOL CMainWndSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CTabDlg::PreTranslateMessage(pMsg);
}


void CMainWndSettingsDlg::OnOK()
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
    GetDlgItemText(IDC_FONT_NAME_EDIT, m_data.font.name);

    CTabDlg::OnOK();
}


void CMainWndSettingsDlg::OnBnClickedHidePercentageCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.hide_percent = (((CButton*)GetDlgItem(IDC_HIDE_PERCENTAGE_CHECK))->GetCheck() != 0);
}


afx_msg LRESULT CMainWndSettingsDlg::OnStaticClicked(WPARAM wParam, LPARAM lParam)
{
    switch (::GetDlgCtrlID(((CWnd*)wParam)->m_hWnd))
    {
    case IDC_TEXT_COLOR_STATIC:
    {
        //设置文本颜色
        if (m_data.specify_each_item_color)
        {
            CMainWndColorDlg colorDlg(m_data.text_colors);
            if (colorDlg.DoModal() == IDOK)
            {
                m_data.text_colors = colorDlg.GetColors();
                DrawStaticColor();
            }
        }
        else if (!m_data.text_colors.empty())
        {
            CMFCColorDialogEx colorDlg(m_data.text_colors.begin()->second, 0, this);
            if (colorDlg.DoModal() == IDOK)
            {
                m_data.text_colors.begin()->second = colorDlg.GetColor();
                DrawStaticColor();
            }
        }
        break;
    }
    default:
        break;
    }
    return 0;
}


void CMainWndSettingsDlg::OnBnClickedSpecifyEachItemColorCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.specify_each_item_color = (((CButton*)GetDlgItem(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK))->GetCheck() != 0);
    DrawStaticColor();
}


void CMainWndSettingsDlg::OnCbnSelchangeDoubleClickCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.double_click_action = static_cast<DoubleClickAction>(m_double_click_combo.GetCurSel());
    EnableControl();
}


void CMainWndSettingsDlg::OnBnClickedSeparateValueUnitCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.separate_value_unit_with_space = (((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->GetCheck() != 0);
}


void CMainWndSettingsDlg::OnBnClickedUnitByteRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.unit_byte = true;
    IniUnitCombo();
}


void CMainWndSettingsDlg::OnBnClickedUnitBitRadio()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.unit_byte = false;
    IniUnitCombo();
}


void CMainWndSettingsDlg::OnBnClickedShowToolTipChk()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.show_tool_tip = (((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->GetCheck() != 0);
}


void CMainWndSettingsDlg::OnBnClickedBrowseButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CString szFilter = CCommon::LoadText(IDS_EXE_FILTER);
    CFileDialogEx fileDlg(TRUE, NULL, szFilter);
    if (IDOK == fileDlg.DoModal())
    {
        m_data.double_click_exe = fileDlg.GetPathName();
        SetDlgItemText(IDC_EXE_PATH_EDIT, m_data.double_click_exe.c_str());
    }
}


void CMainWndSettingsDlg::OnBnClickedDisplayTextSettingButton()
{
    // TODO: 在此添加控件通知处理程序代码
    CDisplayTextSettingDlg dlg(m_data.disp_str, true);
    dlg.DoModal();
}


void CMainWndSettingsDlg::OnCbnSelchangeMemoryDisplayCombo()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.memory_display = static_cast<MemoryDisplay>(m_memory_display_combo.GetCurSel());
}


void CMainWndSettingsDlg::OnBnClickedAlwaysOnTopCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.m_always_on_top = IsDlgButtonChecked(IDC_ALWAYS_ON_TOP_CHECK) != 0;
}


void CMainWndSettingsDlg::OnBnClickedMousePenetrateCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.m_mouse_penetrate = IsDlgButtonChecked(IDC_MOUSE_PENETRATE_CHECK) != 0;
}


void CMainWndSettingsDlg::OnBnClickedLockWindowPosCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.m_lock_window_pos = IsDlgButtonChecked(IDC_LOCK_WINDOW_POS_CHECK) != 0;
}


void CMainWndSettingsDlg::OnBnClickedAlowOutOfBorderCheck()
{
    // TODO: 在此添加控件通知处理程序代码
    m_data.m_alow_out_of_border = IsDlgButtonChecked(IDC_ALOW_OUT_OF_BORDER_CHECK) != 0;
}
