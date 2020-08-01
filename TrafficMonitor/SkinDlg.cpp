// SkinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "SkinDlg.h"
#include "afxdialogex.h"


// CSkinDlg 对话框

IMPLEMENT_DYNAMIC(CSkinDlg, CBaseDialog)

CSkinDlg::CSkinDlg(CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_SKIN_DIALOG, pParent)
{

}

CSkinDlg::~CSkinDlg()
{
}

CString CSkinDlg::GetDialogName() const
{
	return _T("SkinDlg");
}

void CSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_select_box);
	DDX_Control(pDX, IDC_SKIN_COURSE_STATIC, m_skin_course);
	DDX_Control(pDX, IDC_SKIN_DOWNLOAD_STATIC, m_skin_download);
	DDX_Control(pDX, IDC_PREVIEW_GROUP_STATIC, m_preview_static);
	DDX_Control(pDX, IDC_NOTIFY_STATIC, m_notify_static);
}

void CSkinDlg::LoadSkinLayout(const wstring& cfg_path, LayoutData& layout_data)
{
	CIniHelper ini(cfg_path);
	//从ini文件读取皮肤布局，并根据DPI进行缩放
	layout_data.text_height = theApp.DPI(ini.GetInt(_T("layout"), _T("text_height"), 20));
	layout_data.no_text = ini.GetBool(_T("layout"), _T("no_text"), false);

	layout_data.width_l = theApp.DPI(ini.GetInt(_T("layout"), _T("width_l"), 220));
	layout_data.height_l = theApp.DPI(ini.GetInt(_T("layout"), _T("height_l"), 43));
	layout_data.up_x_l = theApp.DPI(ini.GetInt(_T("layout"), _T("up_x_l"), 6));
	layout_data.up_y_l = theApp.DPI(ini.GetInt(_T("layout"), _T("up_y_l"), 2));
	layout_data.up_width_l = theApp.DPI(ini.GetInt(_T("layout"), _T("up_width_l"), 108));
	layout_data.up_align_l = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("up_align_l"), 0));
	layout_data.down_x_l = theApp.DPI(ini.GetInt(_T("layout"), _T("down_x_l"), 114));
	layout_data.down_y_l = theApp.DPI(ini.GetInt(_T("layout"), _T("down_y_l"), 2));
	layout_data.down_width_l = theApp.DPI(ini.GetInt(_T("layout"), _T("down_width_l"), 110));
	layout_data.down_align_l = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("down_align_l"), 0));
	layout_data.cpu_x_l = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_x_l"), 6));
	layout_data.cpu_y_l = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_y_l"), 21));
	layout_data.cpu_width_l = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_width_l"), 108));
	layout_data.cpu_align_l = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("cpu_align_l"), 0));
	layout_data.memory_x_l = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_x_l"), 114));
	layout_data.memory_y_l = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_y_l"), 21));
	layout_data.memory_width_l = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_width_l"), 110));
	layout_data.memory_align_l = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("memory_align_l"), 0));
	layout_data.show_up_l = ini.GetBool(_T("layout"), _T("show_up_l"), true);
	layout_data.show_down_l = ini.GetBool(_T("layout"), _T("show_down_l"), true);
	layout_data.show_cpu_l = ini.GetBool(_T("layout"), _T("show_cpu_l"), true);
	layout_data.show_memory_l = ini.GetBool(_T("layout"), _T("show_memory_l"), true);
	layout_data.preview_x_l = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_x_l"), 0));
	layout_data.preview_y_l = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_y_l"), 47));

	layout_data.width_s = theApp.DPI(ini.GetInt(_T("layout"), _T("width_s"), 220));
	layout_data.height_s = theApp.DPI(ini.GetInt(_T("layout"), _T("height_s"), 28));
	layout_data.up_x_s = theApp.DPI(ini.GetInt(_T("layout"), _T("up_x_s"), 6));
	layout_data.up_y_s = theApp.DPI(ini.GetInt(_T("layout"), _T("up_y_s"), 4));
	layout_data.up_width_s = theApp.DPI(ini.GetInt(_T("layout"), _T("up_width_s"), 108));
	layout_data.up_align_s = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("up_align_s"), 0));
	layout_data.down_x_s = theApp.DPI(ini.GetInt(_T("layout"), _T("down_x_s"), 114));
	layout_data.down_y_s = theApp.DPI(ini.GetInt(_T("layout"), _T("down_y_s"), 4));
	layout_data.down_width_s = theApp.DPI(ini.GetInt(_T("layout"), _T("down_width_s"), 110));
	layout_data.down_align_s = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("down_align_s"), 0));
	layout_data.cpu_x_s = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_x_s"), 0));
	layout_data.cpu_y_s = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_y_s"), 0));
	layout_data.cpu_width_s = theApp.DPI(ini.GetInt(_T("layout"), _T("cpu_width_s"), 0));
	layout_data.cpu_align_s = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("cpu_align_s"), 0));
	layout_data.memory_x_s = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_x_s"), 0));
	layout_data.memory_y_s = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_y_s"), 0));
	layout_data.memory_width_s = theApp.DPI(ini.GetInt(_T("layout"), _T("memory_width_s"), 0));
	layout_data.memory_align_s = static_cast<Alignment>(ini.GetInt(_T("layout"), _T("memory_align_s"), 0));
	layout_data.show_up_s = ini.GetBool(_T("layout"), _T("show_up_s"), true);
	layout_data.show_down_s = ini.GetBool(_T("layout"), _T("show_down_s"), true);
	layout_data.show_cpu_s = ini.GetBool(_T("layout"), _T("show_cpu_s"), false);
	layout_data.show_memory_s = ini.GetBool(_T("layout"), _T("show_memory_s"), false);
	layout_data.preview_x_s = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_x_s"), 0));
	layout_data.preview_y_s = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_y_s"), 0));
}

void CSkinDlg::ShowPreview()
{
	//载入布局数据
	wstring cfg_path{ theApp.m_skin_path + m_skins[m_skin_selected] + L"\\skin.ini" };
	LoadSkinLayout(cfg_path, m_skin_data.layout);
	CIniHelper ini(cfg_path);
	//载入背景图
	LoadBackImage((theApp.m_skin_path + m_skins[m_skin_selected]).c_str(), true);
	LoadBackImage((theApp.m_skin_path + m_skins[m_skin_selected]).c_str(), false);
	//获取当前皮肤的文字颜色
	//m_skin_data.text_color = ini.GetInt(_T("skin"), _T("text_color"), 0);
	ini.GetIntArray(_T("skin"), _T("text_color"), (int*)m_skin_data.text_colors, MAIN_WND_COLOR_NUM, 0);
	m_skin_data.specify_each_item_color = ini.GetBool(_T("skin"), _T("specify_each_item_color"), false);
	//获取当前皮肤的字体
	//m_skin_data.font_name = ini.GetString(L"skin", L"font_name", L"");
	//m_skin_data.font_size = ini.GetInt(_T("skin"), _T("font_size"), 0);
	FontInfo default_font{};
	ini.LoadFontData(L"skin", m_skin_data.font, default_font);
	m_view->IniFont();
	//获取皮肤作者
	wstring skin_author;
	skin_author = ini.GetString(_T("skin"), _T("skin_author"), _T("unknow"));
	SetDlgItemText(IDC_SKIN_INFO, CCommon::LoadText(IDS_SKIN_AUTHOUR, skin_author.c_str()));
	//获取显示文本
	m_skin_data.disp_str.up = ini.GetString(_T("skin"), _T("up_string"), NONE_STR);
	m_skin_data.disp_str.down = ini.GetString(_T("skin"), _T("down_string"), NONE_STR);
	m_skin_data.disp_str.cpu = ini.GetString(_T("skin"), _T("cpu_string"), NONE_STR);
	m_skin_data.disp_str.memory = ini.GetString(_T("skin"), _T("memory_string"), NONE_STR);
	//获取预览区大小
	m_skin_data.layout.preview_width = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_width"), 238));
	m_skin_data.layout.preview_height = theApp.DPI(ini.GetInt(_T("layout"), _T("preview_height"), 105));
	m_view->SetSize(m_skin_data.layout.preview_width, m_skin_data.layout.preview_height);
	//刷新预览图
	m_view->Invalidate();
	//设置提示信息
	bool cover_font_setting{ !m_skin_data.font.name.IsEmpty() || (m_skin_data.font.size >= MIN_FONT_SIZE && m_skin_data.font.size <= MAX_FONT_SIZE) };
	bool cover_str_setting{ !m_skin_data.disp_str.IsInvalid() };
	cover_font_setting = cover_font_setting && theApp.m_general_data.allow_skin_cover_font;
	cover_str_setting = cover_str_setting && theApp.m_general_data.allow_skin_cover_text;
	if (cover_font_setting && cover_str_setting)
		m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_FONT_TEXT_WARNING));
	else if(cover_font_setting)
		m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_FONT_WARNING));
	else if(cover_str_setting)
		m_notify_static.SetWindowTextEx(CCommon::LoadText(IDS_OVERWRITE_TEXT_WARNING));
	else
		m_notify_static.SetWindowTextEx(_T(""));
}


void CSkinDlg::LoadBackImage(const wstring & path, bool small_image)
{
	if (small_image)
	{
		m_background_s.Destroy();
		m_background_s.Load((path + BACKGROUND_IMAGE_S).c_str());
	}
	else
	{
		m_background_l.Destroy();
		m_background_l.Load((path + BACKGROUND_IMAGE_L).c_str());
	}
}

CRect CSkinDlg::CalculateViewRect()
{
	CRect rect;
	m_preview_static.GetWindowRect(rect);		//获取“预览” group box 的位置
	ScreenToClient(&rect);
	CRect scroll_view_rect{ rect };
	scroll_view_rect.DeflateRect(theApp.DPI(12), theApp.DPI(40));
	scroll_view_rect.top = rect.top + theApp.DPI(28);
	return scroll_view_rect;
}


BEGIN_MESSAGE_MAP(CSkinDlg, CBaseDialog)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSkinDlg::OnCbnSelchangeCombo1)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CSkinDlg 消息处理程序

BOOL CSkinDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(CCommon::LoadText(IDS_TITLE_CHANGE_SKIN));
	SetIcon(theApp.GetMenuIcon(IDI_SKIN), FALSE);		// 设置小图标
	//初始化选择框
	for (const auto& skin_path : m_skins)
	{
		wstring skin_name;
		size_t index = skin_path.find_last_of(L'\\');
		skin_name = skin_path.substr(index + 1);
		m_select_box.AddString(skin_name.c_str());
	}
	m_select_box.SetCurSel(m_skin_selected);
	m_select_box.SetMinVisibleItems(9);
	//初始化预览视图
	m_view = (CSkinPreviewView*)RUNTIME_CLASS(CSkinPreviewView)->CreateObject();
	m_view->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, CalculateViewRect(), this, 3000);
	m_view->InitialUpdate();
	m_view->SetSkinData(&m_skin_data);
	m_view->SetBackImage(&m_background_s, &m_background_l);
	m_view->SetFont(m_pFont);
	m_view->IniFont();
	m_view->ShowWindow(SW_SHOW);

	//设置提示信息
	m_notify_static.SetTextColor(RGB(252, 128, 45));
	m_notify_static.SetBackColor(GetSysColor(COLOR_BTNFACE));
	m_notify_static.SetWindowTextEx(_T(""));

	//显示预览图片
	ShowPreview();

	//设置超链接
	m_skin_course.SetURL(_T("https://github.com/zhongyang219/TrafficMonitor/blob/master/皮肤制作教程.md"));
	m_skin_download.SetURL(_T("https://github.com/zhongyang219/TrafficMonitorSkin/blob/master/皮肤下载.md"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSkinDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_skin_selected = m_select_box.GetCurSel();
	ShowPreview();
}


void CSkinDlg::OnSize(UINT nType, int cx, int cy)
{
	CBaseDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_preview_static.m_hWnd != NULL && nType != SIZE_MINIMIZED && m_view != nullptr)
		m_view->MoveWindow(CalculateViewRect());
}
