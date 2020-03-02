
// TrafficMonitor.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TrafficMonitorDlg.h"
#include "crashtool.h"
#include "UpdateHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTrafficMonitorApp

BEGIN_MESSAGE_MAP(CTrafficMonitorApp, CWinApp)
	//ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
	ON_COMMAND(ID_HELP, &CTrafficMonitorApp::OnHelp)
END_MESSAGE_MAP()


// CTrafficMonitorApp ����
CTrafficMonitorApp* CTrafficMonitorApp::self = NULL;

CTrafficMonitorApp::CTrafficMonitorApp()
{
	self = this;
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
	CRASHREPORT::StartCrashReport();
}

void CTrafficMonitorApp::LoadConfig()
{
	CIniHelper ini{ m_config_path };

	//��������
	m_general_data.check_update_when_start = ini.GetBool(_T("general"), _T("check_update_when_start"), true);
	m_general_data.allow_skin_cover_font = ini.GetBool(_T("general"), _T("allow_skin_cover_font"), true);
	m_general_data.allow_skin_cover_text = ini.GetBool(_T("general"), _T("allow_skin_cover_text"), true);
	m_general_data.language = static_cast<Language>(ini.GetInt(_T("general"), _T("language"), 0));
	m_general_data.show_all_interface = ini.GetBool(L"general", L"show_all_interface", false);

	//Windows10��ɫģʽ����
	bool is_windows10_light_theme = m_win_version.IsWindows10LightTheme();
	if (is_windows10_light_theme)
		CCommon::SetColorMode(ColorMode::Light);
	else
		CCommon::SetColorMode(ColorMode::Default);

	//����������
	m_cfg_data.m_transparency = ini.GetInt(_T("config"), _T("transparency"), 80);
	m_cfg_data.m_always_on_top = ini.GetBool(_T("config"), _T("always_on_top"), true);
	m_cfg_data.m_lock_window_pos = ini.GetBool(_T("config"), _T("lock_window_pos"), false);
	m_cfg_data.m_show_notify_icon = ini.GetBool(_T("config"), _T("show_notify_icon"), true);
	m_cfg_data.m_show_more_info = ini.GetBool(_T("config"), _T("show_cpu_memory"), false);
	m_cfg_data.m_mouse_penetrate = ini.GetBool(_T("config"), _T("mouse_penetrate"), false);
	m_cfg_data.m_show_task_bar_wnd = ini.GetBool(_T("config"), _T("show_task_bar_wnd"), false);
	m_cfg_data.m_position_x = ini.GetInt(_T("config"), _T("position_x"), -1);
	m_cfg_data.m_position_y = ini.GetInt(_T("config"), _T("position_y"), -1);
	m_cfg_data.m_auto_select = ini.GetBool(_T("connection"), _T("auto_select"), true);
	m_cfg_data.m_select_all = ini.GetBool(_T("connection"), _T("select_all"), false);
	//m_main_wnd_data.text_color = ini.GetInt(_T("config"), _T("text_color"), 16384);
	ini.GetIntArray(_T("config"), _T("text_color"), (int*)m_main_wnd_data.text_colors, MAIN_WND_COLOR_NUM, 16384);
	m_main_wnd_data.specify_each_item_color = ini.GetBool(_T("config"), _T("specify_each_item_color"), false);
	m_cfg_data.m_hide_main_window = ini.GetBool(_T("config"), _T("hide_main_window"), false);
	m_cfg_data.m_connection_name = CCommon::UnicodeToStr(ini.GetString(L"connection", L"connection_name", L"").c_str());
	m_cfg_data.m_skin_name = ini.GetString(_T("config"), _T("skin_selected"), _T(""));
	if (m_cfg_data.m_skin_name.substr(0, 8) == L".\\skins\\")		//�����ȡ����Ƥ������ǰ����".\\skins\\"�������ɾ���������ں�ǰһ���汾���ּ����ԣ�
		m_cfg_data.m_skin_name = m_cfg_data.m_skin_name.substr(7);
	m_cfg_data.m_notify_icon_selected = ini.GetInt(_T("config"), _T("notify_icon_selected"), (m_win_version.IsWindows7() || m_win_version.IsWindows8Or8point1() ? 2 : m_cfg_data.m_dft_notify_icon));		//Win7/8/8.1Ĭ��ʹ����ɫ֪ͨ��ͼ�꣬��Ϊ����֪ͨ��ͼ����ɫͼ��ῴ���壬����ϵͳĬ��ʹ�ð�ɫͼ��
	m_main_wnd_data.swap_up_down = ini.GetBool(_T("config"), _T("swap_up_down"), false);
	m_main_wnd_data.hide_main_wnd_when_fullscreen = ini.GetBool(_T("config"), _T("hide_main_wnd_when_fullscreen"), true);

	FontInfo default_font{};
	default_font.name = CCommon::LoadText(IDS_DEFAULT_FONT);
	default_font.size = 10;
	ini.LoadFontData(_T("config"), m_main_wnd_data.font, default_font);
	//m_main_wnd_data.font.name = ini.GetString(_T("config"), _T("font_name"), CCommon::LoadText(IDS_MICROSOFT_YAHEI)).c_str();
	//m_main_wnd_data.font.size = ini.GetInt(_T("config"), _T("font_size"), 10);

	m_main_wnd_data.disp_str.up = ini.GetString(_T("config"), L"up_string", CCommon::LoadText(IDS_UPLOAD_DISP, _T(": $")));
	m_main_wnd_data.disp_str.down = ini.GetString(L"config", L"down_string", CCommon::LoadText(IDS_DOWNLOAD_DISP, _T(": $")));
	m_main_wnd_data.disp_str.cpu = ini.GetString(L"config", L"cpu_string", L"CPU: $");
	m_main_wnd_data.disp_str.memory = ini.GetString(L"config", L"memory_string", CCommon::LoadText(IDS_MEMORY_DISP, _T(": $")));

	m_main_wnd_data.speed_short_mode = ini.GetBool(_T("config"), _T("speed_short_mode"), false);
	m_main_wnd_data.separate_value_unit_with_space = ini.GetBool(_T("config"), _T("separate_value_unit_with_space"), true);
	m_main_wnd_data.show_tool_tip = ini.GetBool(_T("config"), _T("show_tool_tip"), true);
	m_main_wnd_data.unit_byte = ini.GetBool(_T("config"), _T("unit_byte"), true);
	m_main_wnd_data.speed_unit = static_cast<SpeedUnit>(ini.GetInt(_T("config"), _T("speed_unit"), 0));
	m_main_wnd_data.hide_unit = ini.GetBool(_T("config"), _T("hide_unit"), false);
	m_main_wnd_data.hide_percent = ini.GetBool(_T("config"), _T("hide_percent"), false);
	m_main_wnd_data.double_click_action = static_cast<DoubleClickAction>(ini.GetInt(_T("config"), _T("double_click_action"), 0));

	m_cfg_data.m_alow_out_of_border = ini.GetBool(_T("config"), _T("alow_out_of_border"), false);

	m_general_data.traffic_tip_enable = ini.GetBool(L"notify_tip", L"traffic_tip_enable", false);
	m_general_data.traffic_tip_value = ini.GetInt(L"notify_tip", L"traffic_tip_value", 200);
	m_general_data.traffic_tip_unit = ini.GetInt(L"notify_tip", L"traffic_tip_unit", 0);
	m_general_data.memory_usage_tip_enable = ini.GetBool(L"notify_tip", L"memory_usage_tip_enable", false);
	m_general_data.memory_tip_value = ini.GetInt(L"notify_tip", L"memory_tip_value", 80);


	//��������������
	m_taskbar_data.back_color = ini.GetInt(_T("task_bar"), _T("task_bar_back_color"), m_taskbar_data.dft_back_color);
	m_taskbar_data.transparent_color = ini.GetInt(_T("task_bar"), _T("transparent_color"), m_taskbar_data.dft_transparent_color);
	m_taskbar_data.status_bar_color = ini.GetInt(_T("task_bar"), _T("status_bar_color"), m_taskbar_data.dft_status_bar_color);
	//m_taskbar_data.text_color = GetPrivateProfileInt(_T("task_bar"), _T("task_bar_text_color"), 0x00ffffffU, m_config_path.c_str());
	ini.GetIntArray(_T("task_bar"), _T("task_bar_text_color"), (int*)m_taskbar_data.text_colors, TASKBAR_COLOR_NUM, m_taskbar_data.dft_text_colors);
	m_taskbar_data.specify_each_item_color = ini.GetBool(L"task_bar", L"specify_each_item_color", false);
	m_cfg_data.m_tbar_show_cpu_memory = ini.GetBool(_T("task_bar"), _T("task_bar_show_cpu_memory"), false);
	m_taskbar_data.swap_up_down = ini.GetBool(_T("task_bar"), _T("task_bar_swap_up_down"), false);

	//m_taskbar_data.font.name = ini.GetString(_T("task_bar"), _T("tack_bar_font_name"), CCommon::LoadText(IDS_MICROSOFT_YAHEI)).c_str();
	//m_taskbar_data.font.size = ini.GetInt(_T("task_bar"), _T("tack_bar_font_size"), 9);
	default_font = FontInfo{};
	default_font.name = CCommon::LoadText(IDS_DEFAULT_FONT);
	default_font.size = 9;
	ini.LoadFontData(_T("task_bar"), m_taskbar_data.font, default_font);

	m_taskbar_data.disp_str.up = ini.GetString(L"task_bar", L"up_string", L"��: $");
	m_taskbar_data.disp_str.down = ini.GetString(L"task_bar", L"down_string", L"��: $");
	m_taskbar_data.disp_str.cpu = ini.GetString(L"task_bar", L"cpu_string", L"CPU: $");
	m_taskbar_data.disp_str.memory = ini.GetString(L"task_bar", L"memory_string", CCommon::LoadText(IDS_MEMORY_DISP, _T(": $")));

	m_taskbar_data.tbar_wnd_on_left = ini.GetBool(_T("task_bar"), _T("task_bar_wnd_on_left"), false);
	m_taskbar_data.speed_short_mode = ini.GetBool(_T("task_bar"), _T("task_bar_speed_short_mode"), false);
	m_taskbar_data.unit_byte = ini.GetBool(_T("task_bar"), _T("unit_byte"), true);
	m_taskbar_data.speed_unit = static_cast<SpeedUnit>(ini.GetInt(_T("task_bar"), _T("task_bar_speed_unit"), 0));
	m_taskbar_data.hide_unit = ini.GetBool(_T("task_bar"), _T("task_bar_hide_unit"), false);
	m_taskbar_data.hide_percent = ini.GetBool(_T("task_bar"), _T("task_bar_hide_percent"), false);
	m_taskbar_data.value_right_align = ini.GetBool(_T("task_bar"), _T("value_right_align"), false);
	m_taskbar_data.horizontal_arrange = ini.GetBool(_T("task_bar"), _T("horizontal_arrange"), false);
	m_taskbar_data.show_status_bar = ini.GetBool(_T("task_bar"), _T("show_status_bar"), true);
	m_taskbar_data.separate_value_unit_with_space = ini.GetBool(_T("task_bar"), _T("separate_value_unit_with_space"), true);
	m_taskbar_data.show_tool_tip = ini.GetBool(_T("task_bar"), _T("show_tool_tip"), true);
	m_taskbar_data.digits_number = ini.GetInt(_T("task_bar"), _T("digits_number"), 4);
	m_taskbar_data.double_click_action = static_cast<DoubleClickAction>(ini.GetInt(_T("task_bar"), _T("double_click_action"), 0));
	m_taskbar_data.double_click_exe = ini.GetString(L"task_bar", L"double_click_exe", (theApp.m_system_dir + L"\\Taskmgr.exe").c_str());

	//��������
	m_cfg_data.m_show_internet_ip = ini.GetBool(L"connection_details", L"show_internet_ip", false);
	m_cfg_data.m_use_log_scale = ini.GetBool(_T("histroy_traffic"), _T("use_log_scale"), true);

	m_no_multistart_warning = ini.GetBool(_T("other"), _T("no_multistart_warning"), false);
	m_notify_interval = ini.GetInt(_T("other"), _T("notify_interval"), 60);
	m_exit_when_start_by_restart_manager = ini.GetBool(_T("other"), _T("exit_when_start_by_restart_manager"), true);
	m_debug_log = ini.GetBool(_T("other"), _T("debug_log"), false);
	//����Win7ϵͳ����������������͸��ɫ�ᵼ�����������ڲ��ɼ������Ĭ����Win7�н���͸��ɫ���趨
	m_taksbar_transparent_color_enable = ini.GetBool(L"other", L"taksbar_transparent_color_enable", !m_win_version.IsWindows7());
}

void CTrafficMonitorApp::SaveConfig()
{
	CIniHelper ini{ m_config_path };

	//��������
	ini.WriteBool(_T("general"), _T("check_update_when_start"), m_general_data.check_update_when_start);
	ini.WriteBool(_T("general"), _T("allow_skin_cover_font"), m_general_data.allow_skin_cover_font);
	ini.WriteBool(_T("general"), _T("allow_skin_cover_text"), m_general_data.allow_skin_cover_text);
	ini.WriteInt(_T("general"), _T("language"), static_cast<int>(m_general_data.language));
	ini.WriteBool(L"general", L"show_all_interface", m_general_data.show_all_interface);

	//����������
	ini.WriteInt(L"config", L"transparency", m_cfg_data.m_transparency);
	ini.WriteBool(L"config", L"always_on_top", m_cfg_data.m_always_on_top);
	ini.WriteBool(L"config", L"lock_window_pos", m_cfg_data.m_lock_window_pos);
	ini.WriteBool(L"config", L"show_notify_icon", m_cfg_data.m_show_notify_icon);
	ini.WriteBool(L"config", L"show_cpu_memory", m_cfg_data.m_show_more_info);
	ini.WriteBool(L"config", L"mouse_penetrate", m_cfg_data.m_mouse_penetrate);
	ini.WriteBool(L"config", L"show_task_bar_wnd", m_cfg_data.m_show_task_bar_wnd);
	ini.WriteInt(L"config", L"position_x", m_cfg_data.m_position_x);
	ini.WriteInt(L"config", L"position_y", m_cfg_data.m_position_y);
	ini.WriteBool(L"connection", L"auto_select", m_cfg_data.m_auto_select);
	ini.WriteBool(L"connection", L"select_all", m_cfg_data.m_select_all);
	ini.WriteIntArray(L"config", L"text_color", (int*)m_main_wnd_data.text_colors, MAIN_WND_COLOR_NUM);
	ini.WriteBool(_T("config"), _T("specify_each_item_color"), m_main_wnd_data.specify_each_item_color);
	ini.WriteInt(L"config", L"hide_main_window", m_cfg_data.m_hide_main_window);
	ini.WriteString(L"connection", L"connection_name", CCommon::StrToUnicode(m_cfg_data.m_connection_name.c_str()).c_str());
	ini.WriteString(_T("config"), _T("skin_selected"), m_cfg_data.m_skin_name.c_str());
	ini.WriteInt(L"config", L"notify_icon_selected", m_cfg_data.m_notify_icon_selected);

	ini.SaveFontData(L"config", m_main_wnd_data.font);

	ini.WriteBool(L"config", L"swap_up_down", m_main_wnd_data.swap_up_down);
	ini.WriteBool(L"config", L"hide_main_wnd_when_fullscreen", m_main_wnd_data.hide_main_wnd_when_fullscreen);

	ini.WriteString(_T("config"), _T("up_string"), m_main_wnd_data.disp_str.up);
	ini.WriteString(_T("config"), _T("down_string"), m_main_wnd_data.disp_str.down);
	ini.WriteString(_T("config"), _T("cpu_string"), m_main_wnd_data.disp_str.cpu);
	ini.WriteString(_T("config"), _T("memory_string"), m_main_wnd_data.disp_str.memory);

	ini.WriteBool(L"config", L"speed_short_mode", m_main_wnd_data.speed_short_mode);
	ini.WriteBool(L"config", L"separate_value_unit_with_space", m_main_wnd_data.separate_value_unit_with_space);
	ini.WriteBool(L"config", L"show_tool_tip", m_main_wnd_data.show_tool_tip);
	ini.WriteBool(L"config", L"unit_byte", m_main_wnd_data.unit_byte);
	ini.WriteInt(L"config", L"speed_unit", static_cast<int>(m_main_wnd_data.speed_unit));
	ini.WriteBool(L"config", L"hide_unit", m_main_wnd_data.hide_unit);
	ini.WriteBool(L"config", L"hide_percent", m_main_wnd_data.hide_percent);
	ini.WriteInt(L"config", L"double_click_action", static_cast<int>(m_main_wnd_data.double_click_action));

	ini.WriteInt(L"config", L"alow_out_of_border", m_cfg_data.m_alow_out_of_border);

	ini.WriteBool(L"notify_tip", L"traffic_tip_enable", m_general_data.traffic_tip_enable);
	ini.WriteInt(L"notify_tip", L"traffic_tip_value", m_general_data.traffic_tip_value);
	ini.WriteInt(L"notify_tip", L"traffic_tip_unit", m_general_data.traffic_tip_unit);
	ini.WriteBool(L"notify_tip", L"memory_usage_tip_enable", m_general_data.memory_usage_tip_enable);
	ini.WriteInt(L"notify_tip", L"memory_tip_value", m_general_data.memory_tip_value);


	//��������������
	ini.WriteInt(L"task_bar", L"task_bar_back_color", m_taskbar_data.back_color);
	ini.WriteInt(L"task_bar", L"transparent_color", m_taskbar_data.transparent_color);
	ini.WriteInt(L"task_bar", L"status_bar_color", m_taskbar_data.status_bar_color);
	ini.WriteIntArray(L"task_bar", L"task_bar_text_color", (int*)m_taskbar_data.text_colors, TASKBAR_COLOR_NUM);
	ini.WriteBool(L"task_bar", L"specify_each_item_color", m_taskbar_data.specify_each_item_color);
	ini.WriteBool(L"task_bar", L"task_bar_show_cpu_memory", m_cfg_data.m_tbar_show_cpu_memory);
	ini.SaveFontData(L"task_bar", m_taskbar_data.font);
	ini.WriteBool(L"task_bar", L"task_bar_swap_up_down", m_taskbar_data.swap_up_down);

	ini.WriteString(_T("task_bar"), _T("up_string"), m_taskbar_data.disp_str.up);
	ini.WriteString(_T("task_bar"), _T("down_string"), m_taskbar_data.disp_str.down);
	ini.WriteString(_T("task_bar"), _T("cpu_string"), m_taskbar_data.disp_str.cpu);
	ini.WriteString(_T("task_bar"), _T("memory_string"), m_taskbar_data.disp_str.memory);

	ini.WriteBool(L"task_bar", L"task_bar_wnd_on_left", m_taskbar_data.tbar_wnd_on_left);
	ini.WriteBool(L"task_bar", L"task_bar_speed_short_mode", m_taskbar_data.speed_short_mode);
	ini.WriteBool(L"task_bar", L"unit_byte", m_taskbar_data.unit_byte);
	ini.WriteInt(L"task_bar", L"task_bar_speed_unit", static_cast<int>(m_taskbar_data.speed_unit));
	ini.WriteBool(L"task_bar", L"task_bar_hide_unit", m_taskbar_data.hide_unit);
	ini.WriteBool(L"task_bar", L"task_bar_hide_percent", m_taskbar_data.hide_percent);
	ini.WriteBool(L"task_bar", L"value_right_align", m_taskbar_data.value_right_align);
	ini.WriteBool(L"task_bar", L"horizontal_arrange", m_taskbar_data.horizontal_arrange);
	ini.WriteBool(L"task_bar", L"show_status_bar", m_taskbar_data.show_status_bar);
	ini.WriteBool(L"task_bar", L"separate_value_unit_with_space", m_taskbar_data.separate_value_unit_with_space);
	ini.WriteBool(L"task_bar", L"show_tool_tip", m_taskbar_data.show_tool_tip);
	ini.WriteInt(L"task_bar", L"digits_number", m_taskbar_data.digits_number);
	ini.WriteInt(L"task_bar", L"double_click_action", static_cast<int>(m_taskbar_data.double_click_action));

	//��������
	ini.WriteBool(L"connection_details", L"show_internet_ip", m_cfg_data.m_show_internet_ip);
	ini.WriteBool(L"histroy_traffic", L"use_log_scale", m_cfg_data.m_use_log_scale);

	ini.WriteBool(_T("other"), _T("no_multistart_warning"), m_no_multistart_warning);
	ini.WriteBool(_T("other"), _T("exit_when_start_by_restart_manager"), m_exit_when_start_by_restart_manager);
	ini.WriteInt(_T("other"), _T("notify_interval"), m_notify_interval);
	ini.WriteBool(_T("other"), _T("taksbar_transparent_color_enable"), m_taksbar_transparent_color_enable);

	ini.WriteString(L"app", L"version", VERSION);

	//����Ƿ񱣴�ɹ�
	if (!ini.Save())
	{
		if (m_cannot_save_config_warning)
		{
			CString info;
			info.LoadString(IDS_CONNOT_SAVE_CONFIG_WARNING);
			info.Replace(_T("<%file_path%>"), m_config_path.c_str());
			AfxMessageBox(info, MB_ICONWARNING);
		}
		m_cannot_save_config_warning = false;
		return;
	}
}

void CTrafficMonitorApp::LoadGlobalConfig()
{
	bool portable_mode_default{ false };
	wstring global_cfg_path{ m_module_dir + L"global_cfg.ini" };
	if (!CCommon::FileExist(global_cfg_path.c_str()))		//���global_cfg.ini�����ڣ������AppData/Roaming/TrafficMonitorĿ¼���Ƿ����config.ini���ж������ļ��ı���λ��
	{
		portable_mode_default = !CCommon::FileExist((m_appdata_dir + L"config.ini").c_str());
	}

	CIniHelper ini{ global_cfg_path };
	m_general_data.portable_mode = ini.GetBool(L"config", L"portable_mode", portable_mode_default);
}

void CTrafficMonitorApp::SaveGlobalConfig()
{
	CIniHelper ini{ m_module_dir + L"global_cfg.ini" };
	ini.WriteBool(L"config", L"portable_mode", m_general_data.portable_mode);

	//����Ƿ񱣴�ɹ�
	if (!ini.Save())
	{
		if (m_cannot_save_global_config_warning)
		{
			CString info;
			info.LoadString(IDS_CONNOT_SAVE_CONFIG_WARNING);
			info.Replace(_T("<%file_path%>"), m_module_dir.c_str());
			AfxMessageBox(info, MB_ICONWARNING);
		}
		m_cannot_save_global_config_warning = false;
		return;
	}
}

int CTrafficMonitorApp::DPI(int pixel)
{
	return m_dpi * pixel / 96;
}

void CTrafficMonitorApp::DPI(CRect & rect)
{
	rect.left = DPI(rect.left);
	rect.right = DPI(rect.right);
	rect.top = DPI(rect.top);
	rect.bottom = DPI(rect.bottom);
}

void CTrafficMonitorApp::GetDPI(CWnd* pWnd)
{
	CWindowDC dc(pWnd);
	HDC hDC = dc.GetSafeHdc();
	m_dpi = GetDeviceCaps(hDC, LOGPIXELSY);
}

void CTrafficMonitorApp::CheckUpdate(bool message)
{
	CWaitCursor wait_cursor;

	wstring version;		//����汾
	wstring link;			//��������
    wstring contents_zh_cn;	//�������ݣ��������ģ�
    wstring contents_en;	//�������ݣ�English��
    wstring contents_zh_tw;	//�������ݣ��������ģ�
	CUpdateHelper update_helper;
    if (!update_helper.CheckForUpdate())
    {
        if (message)
            AfxMessageBox(CCommon::LoadText(IDS_CHECK_UPDATE_FAILD), MB_OK | MB_ICONWARNING);
        return;
    }
    version = update_helper.GetVersion();
#ifdef _M_X64
	link = update_helper.GetLink64();
#else
	link = update_helper.GetLink();
#endif
	contents_zh_cn = update_helper.GetContentsZhCn();
	contents_en = update_helper.GetContentsEn();
	contents_zh_tw = update_helper.GetContentsZhTw();
	if (version.empty() || link.empty())
	{
		if (message)
        {
            CString info = CCommon::LoadText(IDS_CHECK_UPDATE_ERROR);
            info += _T("\r\nrow_data=");
            info += std::to_wstring(update_helper.IsRowData()).c_str();

            AfxMessageBox(info, MB_OK | MB_ICONWARNING);
        }
		return;
	}
	if (version > VERSION)		//����������ϵİ汾���ڱ��ذ汾
	{
		CString info;
		//������������ѡ���Ӧ���԰汾�ĸ�������
		int language_code = _ttoi(CCommon::LoadText(IDS_LANGUAGE_CODE));
		wstring contents_lan;
		switch (language_code)
		{
		case 2: contents_lan = contents_zh_cn; break;
		case 3: contents_lan = contents_zh_tw; break;
		default: contents_lan = contents_en; break;
		}

		if (contents_lan.empty())
			info.Format(CCommon::LoadText(IDS_UPDATE_AVLIABLE), version.c_str());
		else
			info.Format(CCommon::LoadText(IDS_UPDATE_AVLIABLE2), version.c_str(), contents_lan.c_str());
			
		if (AfxMessageBox(info, MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			ShellExecute(NULL, _T("open"), link.c_str(), NULL, NULL, SW_SHOW);		//ת����������
		}
	}
	else
	{
		if(message)
			AfxMessageBox(CCommon::LoadText(IDS_ALREADY_UPDATED), MB_OK | MB_ICONINFORMATION);
	}
}

UINT CTrafficMonitorApp::CheckUpdateThreadFunc(LPVOID lpParam)
{
	CCommon::SetThreadLanguage(theApp.m_general_data.language);		//�����߳�����
	CheckUpdate(false);		//������
	return 0;
}

void CTrafficMonitorApp::SetAutoRun(bool auto_run)
{
	CRegKey key;
	//��ע�����
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
	{
		AfxMessageBox(CCommon::LoadText(IDS_AUTORUN_FAILED_NO_KEY), MB_OK | MB_ICONWARNING);
		return;
	}
	if (auto_run)		//д��ע�����
	{
		if (key.SetStringValue(_T("TrafficMonitor"), m_module_path_reg.c_str()) != ERROR_SUCCESS)
		{
			AfxMessageBox(CCommon::LoadText(IDS_AUTORUN_FAILED_NO_ACCESS), MB_OK | MB_ICONWARNING);
			return;
		}
	}
	else		//ɾ��ע�����
	{
		//ɾ��ǰ�ȼ��ע������Ƿ���ڣ���������ڣ���ֱ�ӷ���
		wchar_t buff[256];
		ULONG size{ 256 };
		if (key.QueryStringValue(_T("TrafficMonitor"), buff, &size) != ERROR_SUCCESS)
			return;
		if (key.DeleteValue(_T("TrafficMonitor")) != ERROR_SUCCESS)
		{
			AfxMessageBox(CCommon::LoadText(IDS_AUTORUN_DELETE_FAILED), MB_OK | MB_ICONWARNING);
			return;
		}
	}
}

bool CTrafficMonitorApp::GetAutoRun()
{
	CRegKey key;
	if (key.Open(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run")) != ERROR_SUCCESS)
	{
		//��ע���Software\\Microsoft\\Windows\\CurrentVersion\\Run��ʧ�ܣ��򷵻�false
		return false;
	}
	wchar_t buff[256];
	ULONG size{ 256 };
	if (key.QueryStringValue(_T("TrafficMonitor"), buff, &size) == ERROR_SUCCESS)		//����ҵ��ˡ�TrafficMonitor����
	{
		return (m_module_path_reg == buff);	//�����TrafficMonitor����ֵ�ǵ�ǰ�����·�����ͷ���true�����򷵻�false
	}
	else
	{
		return false;		//û���ҵ���TrafficMonitor����������false
	}
}

CString CTrafficMonitorApp::GetSystemInfoString()
{
	CString info;
	info += _T("System Info:\r\n");

	CString strTmp;
	strTmp.Format(_T("Windows Version: %d.%d build %d\r\n"), m_win_version.GetMajorVersion(),
		m_win_version.GetMinorVersion(), m_win_version.GetBuildNumber());
	info += strTmp;

	strTmp.Format(_T("DPI: %d"), m_dpi);
	info += strTmp;
	info += _T("\r\n");

	return info;
}


// Ψһ��һ�� CTrafficMonitorApp ����

CTrafficMonitorApp theApp;


// CTrafficMonitorApp ��ʼ��

BOOL CTrafficMonitorApp::InitInstance()
{
	//CSimpleXML xml(L"C:\\Temp\\xmldoc_test1.xml");
	//wstring str;
	//str = xml.GetNode(L"panorama", L"face1");

	//���������ļ���·��
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(NULL, path, MAX_PATH);
	m_module_path = path;
	if (m_module_path.find(L' ') != wstring::npos)
	{
		//���·�����пո����ڳ���·��ǰ�����˫����
		m_module_path_reg = L'\"';
		m_module_path_reg += m_module_path;
		m_module_path_reg += L'\"';
	}
	else
	{
		m_module_path_reg = m_module_path;
	}
	m_module_dir = CCommon::GetModuleDir();
	m_system_dir = CCommon::GetSystemDir();
	m_appdata_dir = CCommon::GetAppDataConfigDir();

	LoadGlobalConfig();

#ifdef _DEBUG
	m_config_dir = L".\\";
	m_skin_path = L".\\skins";
#else
	if (m_general_data.portable_mode)
		m_config_dir = m_module_dir;
	else
		m_config_dir = m_appdata_dir;
	m_skin_path = m_module_dir + L"skins";
#endif
	//AppData����ĳ��������ļ�·��
	m_config_path = m_config_dir + L"config.ini";
	m_history_traffic_path = m_config_dir + L"history_traffic.dat";
	m_log_path = m_config_dir + L"error.log";

//#ifndef _DEBUG
//	//ԭ���ġ���������Ŀ¼�µ������ļ���·��
//	wstring config_path_old = m_module_dir + L"config.ini";
//	wstring history_traffic_path_old = m_module_dir + L"history_traffic.dat";
//	wstring log_path_old = m_module_dir + L"error.log";
//	//�����������Ŀ¼�º��������ļ��������ƶ���AppData��Ӧ��Ŀ¼����
//	CCommon::MoveAFile(config_path_old.c_str(), m_config_path.c_str());
//	CCommon::MoveAFile(history_traffic_path_old.c_str(), m_history_traffic_path.c_str());
//	CCommon::MoveAFile(log_path_old.c_str(), m_log_path.c_str());
//#endif // !_DEBUG

	bool is_windows10_fall_creator = m_win_version.IsWindows10FallCreatorOrLater();

	//��ini�ļ���������
	LoadConfig();

	//��ʼ����������
	CCommon::SetThreadLanguage(m_general_data.language);

#ifndef _DEBUG
	wstring cmd_line{ m_lpCmdLine };
	bool is_restart{ cmd_line.find(L"RestartByRestartManager") != wstring::npos };		//��������в����к����ַ�����RestartByRestartManager����˵�������Ǳ�Windows����������
	//bool when_start{ CCommon::WhenStart(m_no_multistart_warning_time) };
	if (m_exit_when_start_by_restart_manager && is_restart && is_windows10_fall_creator)		//��ǰWindows�汾���＾�����߸���ʱ���������������������ֱ���˳�����
	{
		//AfxMessageBox(_T("������Ϣ�������ѱ�Windows����������������������"));
		return FALSE;
	}

	//����Ƿ�����ʵ����������
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, _T("TrafficMonitor-1419J3XLKL1w8OZc"));
	if (hMutex != NULL)
	{
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			//char buff[128];
			//string cmd_line_str{ CCommon::UnicodeToStr(cmd_line.c_str()) };
			//sprintf_s(buff, "when_start=%d, m_no_multistart_warning=%d, cmd_line=%s", when_start, m_no_multistart_warning, cmd_line_str.c_str());
			//CCommon::WriteLog(buff, _T(".\\start.log"));
			if (!m_no_multistart_warning)
			{
				AfxMessageBox(CCommon::LoadText(IDS_AN_INSTANCE_RUNNING));
			}
			return FALSE;
		}
	}
#endif

	//SaveConfig();

	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()��  ���򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	//����ʱ������
#ifndef _DEBUG		//DEBUG�²�������ʱ������
	if (m_general_data.check_update_when_start)
	{
		m_pUpdateThread = AfxBeginThread(CheckUpdateThreadFunc, NULL);
	}
#endif // !_DEBUG

	CTrafficMonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



void CTrafficMonitorApp::OnHelp()
{
	// TODO: �ڴ���������������
	CString language_code;
	language_code.LoadString(IDS_LANGUAGE_CODE);
	if (language_code == _T("2"))
		ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/TrafficMonitor/blob/master/Help.md"), NULL, NULL, SW_SHOW);	//�򿪳�����;
	else
		ShellExecute(NULL, _T("open"), _T("https://github.com/zhongyang219/TrafficMonitor/blob/master/Help_en-us.md"), NULL, NULL, SW_SHOW);
}
