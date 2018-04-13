// TaskBarSettingsDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarSettingsDlg.h"
#include "afxdialogex.h"


// CTaskBarSettingsDlg 对话框

IMPLEMENT_DYNAMIC(CTaskBarSettingsDlg, CTabDlg)

CTaskBarSettingsDlg::CTaskBarSettingsDlg(CWnd* pParent /*=NULL*/)
	: CTabDlg(IDD_TASKBAR_SETTINGS_DIALOG, pParent)
{

}

CTaskBarSettingsDlg::~CTaskBarSettingsDlg()
{
}

void CTaskBarSettingsDlg::DrawStaticColor()
{
	//CCommon::FillStaticColor(m_text_color_static, m_data.text_color);
	//CCommon::FillStaticColor(m_back_color_static, m_data.back_color);
	m_text_color_static.SetFillColor(m_data.text_color);
	m_back_color_static.SetFillColor(m_data.back_color);
}

void CTaskBarSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TEXT_COLOR_STATIC1, m_text_color_static);
	DDX_Control(pDX, IDC_TEXT_COLOR_STATIC2, m_back_color_static);
	CTabDlg::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UNIT_COMBO, m_unit_combo);
	DDX_Control(pDX, IDC_HIDE_UNIT_CHECK, m_hide_unit_chk);
	DDX_Control(pDX, IDC_FONT_SIZE_EDIT1, m_font_size_edit);
}


BEGIN_MESSAGE_MAP(CTaskBarSettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_SET_FONT_BUTTON1, &CTaskBarSettingsDlg::OnBnClickedSetFontButton1)
	ON_BN_CLICKED(IDC_SET_COLOR_BUTTON2, &CTaskBarSettingsDlg::OnBnClickedSetColorButton2)
	ON_BN_CLICKED(IDC_SET_COLOR_BUTTON3, &CTaskBarSettingsDlg::OnBnClickedSetColorButton3)
	ON_EN_CHANGE(IDC_UPLOAD_EDIT1, &CTaskBarSettingsDlg::OnEnChangeUploadEdit1)
	ON_EN_CHANGE(IDC_DOWNLOAD_EDIT1, &CTaskBarSettingsDlg::OnEnChangeDownloadEdit1)
	ON_EN_CHANGE(IDC_CPU_EDIT1, &CTaskBarSettingsDlg::OnEnChangeCpuEdit1)
	ON_EN_CHANGE(IDC_MEMORY_EDIT1, &CTaskBarSettingsDlg::OnEnChangeMemoryEdit1)
	ON_BN_CLICKED(IDC_SET_DEFAULT_BUTTON1, &CTaskBarSettingsDlg::OnBnClickedSetDefaultButton1)
	ON_BN_CLICKED(IDC_SWITCH_UP_DOWN_CHECK1, &CTaskBarSettingsDlg::OnBnClickedSwitchUpDownCheck1)
	ON_BN_CLICKED(IDC_TASKBAR_WND_ON_LEFT_CHECK, &CTaskBarSettingsDlg::OnBnClickedTaskbarWndOnLeftCheck)
	ON_BN_CLICKED(IDC_SPEED_SHORT_MODE_CHECK, &CTaskBarSettingsDlg::OnBnClickedSpeedShortModeCheck)
	ON_CBN_SELCHANGE(IDC_UNIT_COMBO, &CTaskBarSettingsDlg::OnCbnSelchangeUnitCombo)
	ON_BN_CLICKED(IDC_HIDE_UNIT_CHECK, &CTaskBarSettingsDlg::OnBnClickedHideUnitCheck)
	ON_BN_CLICKED(IDC_VALUE_RIGHT_ALIGN_CHECK, &CTaskBarSettingsDlg::OnBnClickedValueRightAlignCheck)
END_MESSAGE_MAP()


// CTaskBarSettingsDlg 消息处理程序


BOOL CTaskBarSettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  在此添加额外的初始化

	//初始化各控件状态
	SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font_name);
	//wchar_t buff[16];
	//swprintf_s(buff, L"%d", m_data.font_size);
	//SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
	m_font_size_edit.SetRange(5, 72);
	m_font_size_edit.SetValue(m_data.font_size);

	SetDlgItemText(IDC_UPLOAD_EDIT1, m_data.disp_str.up.c_str());
	SetDlgItemText(IDC_DOWNLOAD_EDIT1, m_data.disp_str.down.c_str());
	SetDlgItemText(IDC_CPU_EDIT1, m_data.disp_str.cpu.c_str());
	SetDlgItemText(IDC_MEMORY_EDIT1, m_data.disp_str.memory.c_str());

	((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK1))->SetCheck(m_data.swap_up_down);
	((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->SetCheck(m_data.tbar_wnd_on_left);
	((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK))->SetCheck(m_data.speed_short_mode);
	((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->SetCheck(m_data.value_right_align);

	DrawStaticColor();

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK), _T("勾选后，将减少网速显示的小数点位数，并且单位不显示“B”"));

	m_unit_combo.AddString(_T("自动"));
	m_unit_combo.AddString(_T("固定为 KB/s"));
	m_unit_combo.AddString(_T("固定为 MB/s"));
	m_unit_combo.SetCurSel(static_cast<int>(m_data.m_speed_unit));

	m_hide_unit_chk.SetCheck(m_data.m_hide_unit);
	if (m_data.m_speed_unit == SpeedUnit::AUTO)
	{
		m_hide_unit_chk.SetCheck(FALSE);
		m_data.m_hide_unit = false;
		m_hide_unit_chk.EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTaskBarSettingsDlg::OnBnClickedSetFontButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFont font;
	font.CreatePointFont(m_data.font_size * 10, m_data.font_name);
	LOGFONT lf{};             //LOGFONT变量
	font.GetLogFont(&lf);
	CFontDialog fontDlg(&lf);	//构造字体对话框，初始选择字体为之前字体
	if (IDOK == fontDlg.DoModal())     // 显示字体对话框
	{
		//获取字体信息
		m_data.font_name = fontDlg.GetFaceName();
		m_data.font_size = fontDlg.GetSize() / 10;
		//将字体信息显示出来
		SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font_name);
		wchar_t buff[16];
		swprintf_s(buff, L"%d", m_data.font_size);
		SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
	}
}

//设置文本颜色
void CTaskBarSettingsDlg::OnBnClickedSetColorButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog colorDlg(m_data.text_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_data.text_color = colorDlg.GetColor();
		if (m_data.back_color == m_data.text_color)
			MessageBox(_T("警告：文字颜色和背景色相同！"), NULL, MB_ICONWARNING);
		DrawStaticColor();
	}
}

//设置背景颜色
void CTaskBarSettingsDlg::OnBnClickedSetColorButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	CColorDialog colorDlg(m_data.back_color, 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_data.back_color = colorDlg.GetColor();
		if (m_data.back_color == m_data.text_color)
			MessageBox(_T("警告：背景色和文字颜色相同！"), NULL, MB_ICONWARNING);
		DrawStaticColor();
	}
}


void CTaskBarSettingsDlg::OnEnChangeUploadEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_UPLOAD_EDIT1, tmp);
	m_data.disp_str.up = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeDownloadEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_DOWNLOAD_EDIT1, tmp);
	m_data.disp_str.down = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeCpuEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_CPU_EDIT1, tmp);
	m_data.disp_str.cpu = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeMemoryEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CTabDlg::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString tmp;
	GetDlgItemText(IDC_MEMORY_EDIT1, tmp);
	m_data.disp_str.memory = tmp;
}


void CTaskBarSettingsDlg::OnBnClickedSetDefaultButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.disp_str.up = L"↑: ";
	m_data.disp_str.down = L"↓: ";
	m_data.disp_str.cpu = L"CPU: ";
	m_data.disp_str.memory = L"内存: ";
	SetDlgItemText(IDC_UPLOAD_EDIT1, m_data.disp_str.up.c_str());
	SetDlgItemText(IDC_DOWNLOAD_EDIT1, m_data.disp_str.down.c_str());
	SetDlgItemText(IDC_CPU_EDIT1, m_data.disp_str.cpu.c_str());
	SetDlgItemText(IDC_MEMORY_EDIT1, m_data.disp_str.memory.c_str());
}


void CTaskBarSettingsDlg::OnBnClickedSwitchUpDownCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.swap_up_down = (((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK1))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedTaskbarWndOnLeftCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.tbar_wnd_on_left = (((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->GetCheck() != 0);
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
	m_data.m_speed_unit = static_cast<SpeedUnit>(m_unit_combo.GetCurSel());
	if (m_data.m_speed_unit == SpeedUnit::AUTO)
	{
		m_hide_unit_chk.SetCheck(FALSE);
		m_data.m_hide_unit = false;
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
	m_data.m_hide_unit = (m_hide_unit_chk.GetCheck() != 0);
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
		info.Format(_T("字体大小必须在 %d~%d 之间！"), MIN_FONT_SIZE, MAX_FONT_SIZE);
		MessageBox(info, NULL, MB_OK | MB_ICONWARNING);
	}
	else
	{
		m_data.font_size = font_size;
	}
	GetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font_name);

	CTabDlg::OnOK();
}


void CTaskBarSettingsDlg::OnBnClickedValueRightAlignCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_data.value_right_align = (((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->GetCheck() != 0);
}
