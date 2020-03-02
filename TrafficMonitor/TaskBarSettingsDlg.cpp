// TaskBarSettingsDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskBarSettingsDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"


// CTaskBarSettingsDlg �Ի���

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
	if (m_data.specify_each_item_color)
	{
		m_text_color_static.SetColorNum(TASKBAR_COLOR_NUM);
		for (int i{}; i<TASKBAR_COLOR_NUM; i++)
			m_text_color_static.SetFillColor(i, m_data.text_colors[i]);
		m_text_color_static.Invalidate();
	}
	else
	{
		m_text_color_static.SetFillColor(m_data.text_colors[0]);
	}
	m_back_color_static.SetFillColor(m_data.back_color);
	m_trans_color_static.SetFillColor(m_data.transparent_color);
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
	DDX_Control(pDX, IDC_TRANSPARENT_COLOR_STATIC, m_trans_color_static);
}


BEGIN_MESSAGE_MAP(CTaskBarSettingsDlg, CTabDlg)
	ON_BN_CLICKED(IDC_SET_FONT_BUTTON1, &CTaskBarSettingsDlg::OnBnClickedSetFontButton1)
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
	ON_BN_CLICKED(IDC_SET_LIGHT_MODE_BUTTON, &CTaskBarSettingsDlg::OnBnClickedSetLightMode)
END_MESSAGE_MAP()


// CTaskBarSettingsDlg ��Ϣ�������


BOOL CTaskBarSettingsDlg::OnInitDialog()
{
	CTabDlg::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//��ʼ�����ؼ�״̬
	SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font.name);
	//wchar_t buff[16];
	//swprintf_s(buff, L"%d", m_data.font_size);
	//SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
	m_font_size_edit.SetRange(5, 72);
	m_font_size_edit.SetValue(m_data.font.size);

	SetDlgItemText(IDC_UPLOAD_EDIT1, m_data.disp_str.up.c_str());
	SetDlgItemText(IDC_DOWNLOAD_EDIT1, m_data.disp_str.down.c_str());
	SetDlgItemText(IDC_CPU_EDIT1, m_data.disp_str.cpu.c_str());
	SetDlgItemText(IDC_MEMORY_EDIT1, m_data.disp_str.memory.c_str());

	((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK1))->SetCheck(m_data.swap_up_down);
	((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->SetCheck(m_data.tbar_wnd_on_left);
	((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK))->SetCheck(m_data.speed_short_mode);
	((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->SetCheck(m_data.value_right_align);
	((CButton*)GetDlgItem(IDC_HORIZONTAL_ARRANGE_CHECK))->SetCheck(m_data.horizontal_arrange);
	((CButton*)GetDlgItem(IDC_SHOW_STATUS_BAR_CHECK))->SetCheck(m_data.show_status_bar);
	((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->SetCheck(m_data.separate_value_unit_with_space);
	((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->SetCheck(m_data.show_tool_tip);

	m_text_color_static.SetLinkCursor();
	m_back_color_static.SetLinkCursor();
	m_trans_color_static.SetLinkCursor();
	m_status_bar_color_static.SetLinkCursor();
	DrawStaticColor();

#ifdef COMPILE_FOR_WINXP
	m_trans_color_static.EnableWindow(FALSE);
#endif // COMPILE_FOR_WINXP

	if(theApp.m_win_version.IsWindows7())
		m_trans_color_static.EnableWindow(FALSE);

	m_toolTip.Create(this);
	m_toolTip.SetMaxTipWidth(theApp.DPI(300));
	m_toolTip.AddTool(GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK), CCommon::LoadText(IDS_SPEED_SHORT_MODE_TIP));

	if(m_data.unit_byte)
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

	m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_CONNECTION_DETIAL));
	m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_HISTORICAL_TRAFFIC));
	m_double_click_combo.AddString(CCommon::LoadText(IDS_SHOW_HIDE_CPU_MEMORY));
	m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_OPTION_SETTINGS));
	m_double_click_combo.AddString(CCommon::LoadText(IDS_OPEN_TASK_MANAGER));
	m_double_click_combo.AddString(CCommon::LoadText(IDS_NONE));
	m_double_click_combo.SetCurSel(static_cast<int>(m_data.double_click_action));

	m_digit_number_combo.AddString(_T("3"));
	m_digit_number_combo.AddString(_T("4"));
	m_digit_number_combo.AddString(_T("5"));
	m_digit_number_combo.AddString(_T("6"));
	m_digit_number_combo.AddString(_T("7"));
	m_digit_number_combo.SetCurSel(m_data.digits_number - 3);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CTaskBarSettingsDlg::OnBnClickedSetFontButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	LOGFONT lf{};
	lf.lfHeight = FONTSIZE_TO_LFHEIGHT(m_data.font.size);
	lf.lfWeight = (m_data.font.bold ? FW_BOLD : FW_NORMAL);
	lf.lfItalic = m_data.font.italic;
	lf.lfUnderline = m_data.font.underline;
	lf.lfStrikeOut = m_data.font.strike_out;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	//wcsncpy_s(lf.lfFaceName, m_data.font.name.GetString(), 32);
	CCommon::WStringCopy(lf.lfFaceName, 32, m_data.font.name.GetString());
	CCommon::NormalizeFont(lf);
	CFontDialog fontDlg(&lf);	//��������Ի��򣬳�ʼѡ������Ϊ֮ǰ����
	if (IDOK == fontDlg.DoModal())     // ��ʾ����Ի���
	{
		//��ȡ������Ϣ
		m_data.font.name = fontDlg.GetFaceName();
		m_data.font.size = fontDlg.GetSize() / 10;
		m_data.font.bold = (fontDlg.IsBold() != FALSE);
		m_data.font.italic = (fontDlg.IsItalic() != FALSE);
		m_data.font.underline = (fontDlg.IsUnderline() != FALSE);
		m_data.font.strike_out = (fontDlg.IsStrikeOut() != FALSE);
		//��������Ϣ��ʾ����
		SetDlgItemText(IDC_FONT_NAME_EDIT1, m_data.font.name);
		wchar_t buff[16];
		swprintf_s(buff, L"%d", m_data.font.size);
		SetDlgItemText(IDC_FONT_SIZE_EDIT1, buff);
	}
}


void CTaskBarSettingsDlg::OnEnChangeUploadEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CTabDlg::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString tmp;
	GetDlgItemText(IDC_UPLOAD_EDIT1, tmp);
	m_data.disp_str.up = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeDownloadEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CTabDlg::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString tmp;
	GetDlgItemText(IDC_DOWNLOAD_EDIT1, tmp);
	m_data.disp_str.down = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeCpuEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CTabDlg::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString tmp;
	GetDlgItemText(IDC_CPU_EDIT1, tmp);
	m_data.disp_str.cpu = tmp;
}


void CTaskBarSettingsDlg::OnEnChangeMemoryEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CTabDlg::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString tmp;
	GetDlgItemText(IDC_MEMORY_EDIT1, tmp);
	m_data.disp_str.memory = tmp;
}


void CTaskBarSettingsDlg::OnBnClickedSetDefaultButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.disp_str.up = L"��: ";
	m_data.disp_str.down = L"��: ";
	m_data.disp_str.cpu = L"CPU: ";
	m_data.disp_str.memory = CCommon::LoadText(IDS_MEMORY_DISP, _T(": "));
	SetDlgItemText(IDC_UPLOAD_EDIT1, m_data.disp_str.up.c_str());
	SetDlgItemText(IDC_DOWNLOAD_EDIT1, m_data.disp_str.down.c_str());
	SetDlgItemText(IDC_CPU_EDIT1, m_data.disp_str.cpu.c_str());
	SetDlgItemText(IDC_MEMORY_EDIT1, m_data.disp_str.memory.c_str());
}


void CTaskBarSettingsDlg::OnBnClickedSwitchUpDownCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.swap_up_down = (((CButton*)GetDlgItem(IDC_SWITCH_UP_DOWN_CHECK1))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedTaskbarWndOnLeftCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.tbar_wnd_on_left = (((CButton*)GetDlgItem(IDC_TASKBAR_WND_ON_LEFT_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedSpeedShortModeCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.speed_short_mode = (((CButton*)GetDlgItem(IDC_SPEED_SHORT_MODE_CHECK))->GetCheck() != 0);
}


BOOL CTaskBarSettingsDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_MOUSEMOVE)
		m_toolTip.RelayEvent(pMsg);

	return CTabDlg::PreTranslateMessage(pMsg);
}


void CTaskBarSettingsDlg::OnCbnSelchangeUnitCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.hide_unit = (m_hide_unit_chk.GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	//��ȡ��������
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

	//��ȡ����λ��������
	m_data.digits_number = m_digit_number_combo.GetCurSel() + 3;

	CTabDlg::OnOK();
}


void CTaskBarSettingsDlg::OnBnClickedValueRightAlignCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.value_right_align = (((CButton*)GetDlgItem(IDC_VALUE_RIGHT_ALIGN_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedHidePercentageCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.hide_percent = (((CButton*)GetDlgItem(IDC_HIDE_PERCENTAGE_CHECK))->GetCheck() != 0);
}


afx_msg LRESULT CTaskBarSettingsDlg::OnStaticClicked(WPARAM wParam, LPARAM lParam)
{
	switch (::GetDlgCtrlID(((CWnd*)wParam)->m_hWnd))
	{
	case IDC_TEXT_COLOR_STATIC1:		//������ı���ɫ��ʱ
	{
		//�����ı���ɫ
		if (m_data.specify_each_item_color)
		{
			CTaskbarColorDlg colorDlg(m_data.text_colors);
			if (colorDlg.DoModal() == IDOK)
			{
				for (int i{}; i < TASKBAR_COLOR_NUM; i++)
					m_data.text_colors[i] = colorDlg.GetColors()[i];
				DrawStaticColor();
			}
		}
		else
		{
			CMFCColorDialogEx colorDlg(m_data.text_colors[0], 0, this);
			if (colorDlg.DoModal() == IDOK)
			{
				m_data.text_colors[0] = colorDlg.GetColor();
				if (m_data.back_color == m_data.text_colors[0])
					MessageBox(CCommon::LoadText(IDS_SAME_TEXT_BACK_COLOR_WARNING), NULL, MB_ICONWARNING);
				DrawStaticColor();
			}
		}
		break;
	}
	case IDC_TEXT_COLOR_STATIC2:		//�����������ɫ��ʱ
	{
		//���ñ�����ɫ
		CMFCColorDialogEx colorDlg(m_data.back_color, 0, this);
		if (colorDlg.DoModal() == IDOK)
		{
			m_data.back_color = colorDlg.GetColor();
			if (m_data.back_color == m_data.text_colors[0])
				MessageBox(CCommon::LoadText(IDS_SAME_BACK_TEXT_COLOR_WARNING), NULL, MB_ICONWARNING);
			DrawStaticColor();
		}
		break;
	}
	case IDC_TRANSPARENT_COLOR_STATIC:		//�����͸��ɫ��ʱ
	{
		CMFCColorDialogEx colorDlg(m_data.transparent_color, 0, this);
		if (colorDlg.DoModal() == IDOK)
		{
			m_data.transparent_color = colorDlg.GetColor();
			DrawStaticColor();
		}
		break;
	}
	case IDC_TEXT_COLOR_STATIC3:		//�����״̬����ɫ��ʱ
	{
		CMFCColorDialogEx colorDlg(m_data.status_bar_color, 0, this);
		if (colorDlg.DoModal() == IDOK)
		{
			m_data.status_bar_color = colorDlg.GetColor();
			DrawStaticColor();
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.specify_each_item_color = (((CButton*)GetDlgItem(IDC_SPECIFY_EACH_ITEM_COLOR_CHECK))->GetCheck() != 0);
	DrawStaticColor();
}


void CTaskBarSettingsDlg::OnCbnSelchangeDoubleClickCombo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.double_click_action = static_cast<DoubleClickAction>(m_double_click_combo.GetCurSel());
}


void CTaskBarSettingsDlg::OnBnClickedHorizontalArrangeCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.horizontal_arrange = (((CButton*)GetDlgItem(IDC_HORIZONTAL_ARRANGE_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedShowStatusBarCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.show_status_bar = (((CButton*)GetDlgItem(IDC_SHOW_STATUS_BAR_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedSeparateValueUnitCheck()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.separate_value_unit_with_space = (((CButton*)GetDlgItem(IDC_SEPARATE_VALUE_UNIT_CHECK))->GetCheck() != 0);
}


void CTaskBarSettingsDlg::OnBnClickedUnitByteRadio()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.unit_byte = true;
	IniUnitCombo();
}


void CTaskBarSettingsDlg::OnBnClickedUnitBitRadio()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_data.unit_byte = false;
	IniUnitCombo();
}


void CTaskBarSettingsDlg::OnBnClickedShowToolTipChk()
{
    // TODO: �ڴ���ӿؼ�֪ͨ����������
    m_data.show_tool_tip = (((CButton*)GetDlgItem(IDC_SHOW_TOOL_TIP_CHK))->GetCheck() != 0);
}

void CTaskBarSettingsDlg::OnBnClickedSetLightMode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	for (int i{}; i < TASKBAR_COLOR_NUM; i++)
		m_data.text_colors[i] = RGB(0, 0, 0);
	m_data.back_color = RGB(210, 210, 210);
	m_data.transparent_color = RGB(210, 210, 210);
	m_data.status_bar_color = RGB(165, 165, 165);
	DrawStaticColor();
}
