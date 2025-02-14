// CAutoAdaptSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "CAutoAdaptSettingsDlg.h"
#include "afxdialogex.h"


// CAutoAdaptSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CAutoAdaptSettingsDlg, CBaseDialog)

CAutoAdaptSettingsDlg::CAutoAdaptSettingsDlg(TaskBarSettingData& data, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_ATUO_ADAPT_SETTING_DIALOG, pParent), m_data(data)
{

}

CAutoAdaptSettingsDlg::~CAutoAdaptSettingsDlg()
{
}

void CAutoAdaptSettingsDlg::InitComboBox(CComboBox& combo_box, int style_sel)
{
	for (int i = 0; i < TASKBAR_DEFAULT_STYLE_NUM; i++)
		combo_box.AddString(CCommon::LoadText(IDS_PRESET, std::to_wstring(i + 1).c_str()));
	if (style_sel >= 0 && style_sel < TASKBAR_DEFAULT_STYLE_NUM)
		combo_box.SetCurSel(style_sel);
	else
		combo_box.SetCurSel(0);
}

int CAutoAdaptSettingsDlg::GetComboBoxSel(const CComboBox& combo_box)
{
	int sel = combo_box.GetCurSel();
	if (sel >= 0 && sel < TASKBAR_DEFAULT_STYLE_NUM)
		return sel;
	else
		return 0;
}

void CAutoAdaptSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DARK_MODE_DEFAULT_STYLE_COMBO, m_dark_mode_default_style_combo);
	DDX_Control(pDX, IDC_LIGHT_MODE_DEFAULT_STYLE_COMBO, m_light_mode_default_style_combo);
}

CString CAutoAdaptSettingsDlg::GetDialogName() const
{
    return _T("AutoAdaptSettingsDlg");
}

bool CAutoAdaptSettingsDlg::InitializeControls()
{
    RepositionTextBasedControls({
        { CtrlTextInfo::L1, IDC_DARK_MODE_STATIC },
        { CtrlTextInfo::C0, IDC_DARK_MODE_DEFAULT_STYLE_COMBO },
        { CtrlTextInfo::L1, IDC_LIGHT_MODE_STATIC },
        { CtrlTextInfo::C0, IDC_LIGHT_MODE_DEFAULT_STYLE_COMBO }
    });
    return true;
}


BEGIN_MESSAGE_MAP(CAutoAdaptSettingsDlg, CBaseDialog)
END_MESSAGE_MAP()


// CAutoAdaptSettingsDlg 消息处理程序


BOOL CAutoAdaptSettingsDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitComboBox(m_dark_mode_default_style_combo, m_data.dark_default_style);
	InitComboBox(m_light_mode_default_style_combo, m_data.light_default_style);
    CheckDlgButton(IDC_AUTO_SAVE_TO_PRESET_CHECK, m_data.auto_save_taskbar_color_settings_to_preset);

    m_toolTip.Create(this);
    m_toolTip.SetMaxTipWidth(theApp.DPI(300));
    m_toolTip.AddTool(GetDlgItem(IDC_AUTO_SAVE_TO_PRESET_CHECK), CCommon::LoadText(IDS_AUTO_SAVE_TO_PRESET_TIP));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAutoAdaptSettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//获取控件中的设置
	m_data.dark_default_style = GetComboBoxSel(m_dark_mode_default_style_combo);
	m_data.light_default_style = GetComboBoxSel(m_light_mode_default_style_combo);
    m_data.auto_save_taskbar_color_settings_to_preset = (IsDlgButtonChecked(IDC_AUTO_SAVE_TO_PRESET_CHECK) != 0);

	CBaseDialog::OnOK();
}


BOOL CAutoAdaptSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: 在此添加专用代码和/或调用基类
    if (pMsg->message == WM_MOUSEMOVE)
        m_toolTip.RelayEvent(pMsg);

    return CBaseDialog::PreTranslateMessage(pMsg);
}
