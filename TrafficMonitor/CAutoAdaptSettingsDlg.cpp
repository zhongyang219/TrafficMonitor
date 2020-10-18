// CAutoAdaptSettingsDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "CAutoAdaptSettingsDlg.h"
#include "afxdialogex.h"


// CAutoAdaptSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CAutoAdaptSettingsDlg, CDialog)

CAutoAdaptSettingsDlg::CAutoAdaptSettingsDlg(TaskBarSettingData& data, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_ATUO_ADAPT_SETTING_DIALOG, pParent), m_data(data)
{

}

CAutoAdaptSettingsDlg::~CAutoAdaptSettingsDlg()
{
}

void CAutoAdaptSettingsDlg::InitComboBox(CComboBox& combo_box, int style_sel)
{
	combo_box.AddString(CCommon::LoadText(IDS_LIGHT_MODE));
	for (int i = 0; i < TASKBAR_DEFAULT_STYLE_NUM; i++)
		combo_box.AddString(CCommon::LoadText(IDS_PRESET, std::to_wstring(i + 1).c_str()));
	if (style_sel == TASKBAR_DEFAULT_LIGHT_STYLE_INDEX)
		combo_box.SetCurSel(0);
	else if (style_sel >= 0 && style_sel < TASKBAR_DEFAULT_STYLE_NUM)
		combo_box.SetCurSel(style_sel + 1);
	else
		combo_box.SetCurSel(1);
}

int CAutoAdaptSettingsDlg::GetComboBoxSel(const CComboBox& combo_box)
{
	int sel = combo_box.GetCurSel();
	if (sel == 0)
		return TASKBAR_DEFAULT_LIGHT_STYLE_INDEX;
	else if (sel >= 1 && sel < TASKBAR_DEFAULT_STYLE_NUM + 1)
		return sel - 1;
	else
		return 1;
}

void CAutoAdaptSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DARK_MODE_DEFAULT_STYLE_COMBO, m_dark_mode_default_style_combo);
	DDX_Control(pDX, IDC_LIGHT_MODE_DEFAULT_STYLE_COMBO, m_light_mode_default_style_combo);
}


BEGIN_MESSAGE_MAP(CAutoAdaptSettingsDlg, CDialog)
END_MESSAGE_MAP()


// CAutoAdaptSettingsDlg 消息处理程序


BOOL CAutoAdaptSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitComboBox(m_dark_mode_default_style_combo, m_data.dark_default_style);
	InitComboBox(m_light_mode_default_style_combo, m_data.light_default_style);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAutoAdaptSettingsDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//获取控件中的设置
	m_data.dark_default_style = GetComboBoxSel(m_dark_mode_default_style_combo);
	m_data.light_default_style = GetComboBoxSel(m_light_mode_default_style_combo);

	CDialog::OnOK();
}
