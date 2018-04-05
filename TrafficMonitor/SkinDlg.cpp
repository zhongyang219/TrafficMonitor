// SkinDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "SkinDlg.h"
#include "afxdialogex.h"


// CSkinDlg 对话框

IMPLEMENT_DYNAMIC(CSkinDlg, CDialogEx)

CSkinDlg::CSkinDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SKIN_DIALOG, pParent)
{

}

CSkinDlg::~CSkinDlg()
{
}

void CSkinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_select_box);
	DDX_Control(pDX, IDC_SKIN_COURSE_STATIC, m_skin_course);
	DDX_Control(pDX, IDC_SKIN_DOWNLOAD_STATIC, m_skin_download);
	DDX_Control(pDX, IDC_PREVIEW_GROUP_STATIC, m_preview_static);
	DDX_Control(pDX, IDC_NOTIFY_STATIC, m_notify_static);
}

void CSkinDlg::LoadSkinLayout(const wstring& cfg_path, LayoutData& layout_data)
{
	//从ini文件读取皮肤布局，并根据DPI进行缩放
	layout_data.text_height = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("text_height"), 20, cfg_path.c_str()));
	layout_data.no_text = (GetPrivateProfileInt(_T("layout"), _T("no_text"), 0, cfg_path.c_str()) != 0);

	layout_data.width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("width_l"), 220, cfg_path.c_str()));
	layout_data.height_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("height_l"), 43, cfg_path.c_str()));
	layout_data.up_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_x_l"), 6, cfg_path.c_str()));
	layout_data.up_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_y_l"), 2, cfg_path.c_str()));
	layout_data.up_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_width_l"), 108, cfg_path.c_str()));
	layout_data.up_align_l = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("up_align_l"), 0, cfg_path.c_str()));
	layout_data.down_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_x_l"), 114, cfg_path.c_str()));
	layout_data.down_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_y_l"), 2, cfg_path.c_str()));
	layout_data.down_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_width_l"), 110, cfg_path.c_str()));
	layout_data.down_align_l = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("down_align_l"), 0, cfg_path.c_str()));
	layout_data.cpu_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_x_l"), 6, cfg_path.c_str()));
	layout_data.cpu_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_y_l"), 21, cfg_path.c_str()));
	layout_data.cpu_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_width_l"), 108, cfg_path.c_str()));
	layout_data.cpu_align_l = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("cpu_align_l"), 0, cfg_path.c_str()));
	layout_data.memory_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_x_l"), 114, cfg_path.c_str()));
	layout_data.memory_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_y_l"), 21, cfg_path.c_str()));
	layout_data.memory_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_width_l"), 110, cfg_path.c_str()));
	layout_data.memory_align_l = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("memory_align_l"), 0, cfg_path.c_str()));
	layout_data.show_up_l = (GetPrivateProfileInt(_T("layout"), _T("show_up_l"), 1, cfg_path.c_str()) != 0);
	layout_data.show_down_l = (GetPrivateProfileInt(_T("layout"), _T("show_down_l"), 1, cfg_path.c_str()) != 0);
	layout_data.show_cpu_l = (GetPrivateProfileInt(_T("layout"), _T("show_cpu_l"), 1, cfg_path.c_str()) != 0);
	layout_data.show_memory_l = (GetPrivateProfileInt(_T("layout"), _T("show_memory_l"), 1, cfg_path.c_str()) != 0);
	layout_data.preview_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_x_l"), 0, cfg_path.c_str()));
	layout_data.preview_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_y_l"), 47, cfg_path.c_str()));

	layout_data.width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("width_s"), 220, cfg_path.c_str()));
	layout_data.height_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("height_s"), 28, cfg_path.c_str()));
	layout_data.up_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_x_s"), 6, cfg_path.c_str()));
	layout_data.up_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_y_s"), 4, cfg_path.c_str()));
	layout_data.up_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_width_s"), 108, cfg_path.c_str()));
	layout_data.up_align_s = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("up_align_s"), 0, cfg_path.c_str()));
	layout_data.down_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_x_s"), 114, cfg_path.c_str()));
	layout_data.down_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_y_s"), 4, cfg_path.c_str()));
	layout_data.down_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_width_s"), 110, cfg_path.c_str()));
	layout_data.down_align_s = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("down_align_s"), 0, cfg_path.c_str()));
	layout_data.cpu_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_x_s"), 0, cfg_path.c_str()));
	layout_data.cpu_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_y_s"), 0, cfg_path.c_str()));
	layout_data.cpu_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_width_s"), 0, cfg_path.c_str()));
	layout_data.cpu_align_s = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("cpu_align_s"), 0, cfg_path.c_str()));
	layout_data.memory_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_x_s"), 0, cfg_path.c_str()));
	layout_data.memory_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_y_s"), 0, cfg_path.c_str()));
	layout_data.memory_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_width_s"), 0, cfg_path.c_str()));
	layout_data.memory_align_s = static_cast<Alignment>(GetPrivateProfileInt(_T("layout"), _T("memory_align_s"), 0, cfg_path.c_str()));
	layout_data.show_up_s = (GetPrivateProfileInt(_T("layout"), _T("show_up_s"), 1, cfg_path.c_str()) != 0);
	layout_data.show_down_s = (GetPrivateProfileInt(_T("layout"), _T("show_down_s"), 1, cfg_path.c_str()) != 0);
	layout_data.show_cpu_s = (GetPrivateProfileInt(_T("layout"), _T("show_cpu_s"), 0, cfg_path.c_str()) != 0);
	layout_data.show_memory_s = (GetPrivateProfileInt(_T("layout"), _T("show_memory_s"), 0, cfg_path.c_str()) != 0);
	layout_data.preview_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_x_s"), 0, cfg_path.c_str()));
	layout_data.preview_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_y_s"), 0, cfg_path.c_str()));
}

void CSkinDlg::ShowPreview()
{
	//载入布局数据
	wstring cfg_path{ theApp.m_skin_path + m_skins[m_skin_selected] + L"\\skin.ini" };
	LoadSkinLayout(cfg_path, m_skin_data.layout);
	//载入背景图
	LoadBackImage((theApp.m_skin_path + m_skins[m_skin_selected]).c_str(), true);
	LoadBackImage((theApp.m_skin_path + m_skins[m_skin_selected]).c_str(), false);
	//获取当前皮肤的文字颜色
	m_skin_data.text_color = GetPrivateProfileInt(_T("skin"), _T("text_color"), 0, cfg_path.c_str());
	//获取当前皮肤的字体
	m_skin_data.font_name = CCommon::GetIniStringW(L"skin", L"font_name", L"", cfg_path.c_str());
	m_skin_data.font_size = GetPrivateProfileInt(_T("skin"), _T("font_size"), 0, cfg_path.c_str());
	m_view->IniFont();
	//获取皮肤作者
#define BUFF_SIZE 64
	wchar_t buff[BUFF_SIZE];
	GetPrivateProfileString(_T("skin"), _T("skin_author"), _T("unknow"), buff, BUFF_SIZE, cfg_path.c_str());
	SetDlgItemText(IDC_SKIN_INFO, (wstring(L"皮肤作者：") + buff).c_str());
	//获取显示文本
	m_skin_data.disp_str.up = CCommon::GetIniStringW(_T("skin"), _T("up_string"), NONE_STR, cfg_path.c_str());
	m_skin_data.disp_str.down = CCommon::GetIniStringW(_T("skin"), _T("down_string"), NONE_STR, cfg_path.c_str());
	m_skin_data.disp_str.cpu = CCommon::GetIniStringW(_T("skin"), _T("cpu_string"), NONE_STR, cfg_path.c_str());
	m_skin_data.disp_str.memory = CCommon::GetIniStringW(_T("skin"), _T("memory_string"), NONE_STR, cfg_path.c_str());
	//获取预览区大小
	m_skin_data.layout.preview_width = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_width"), 238, cfg_path.c_str()));
	m_skin_data.layout.preview_height = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("preview_height"), 105, cfg_path.c_str()));
	m_view->SetSize(m_skin_data.layout.preview_width, m_skin_data.layout.preview_height);
	//刷新预览图
	m_view->Invalidate();
	//设置提示信息
	bool cover_font_setting{ !m_skin_data.font_name.empty() || (m_skin_data.font_size >= MIN_FONT_SIZE && m_skin_data.font_size <= MAX_FONT_SIZE) };
	bool cover_str_setting{ !m_skin_data.disp_str.IsInvalid() };
	if (cover_font_setting && cover_str_setting)
		m_notify_static.SetWindowTextEx(_T("注意：此皮肤会覆盖字体设置和显示文本设置。"));
	else if(cover_font_setting)
		m_notify_static.SetWindowTextEx(_T("注意：此皮肤会覆盖字体设置。"));
	else if(cover_str_setting)
		m_notify_static.SetWindowTextEx(_T("注意：此皮肤会覆盖显示文本设置。"));
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


BEGIN_MESSAGE_MAP(CSkinDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSkinDlg::OnCbnSelchangeCombo1)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CSkinDlg 消息处理程序

BOOL CSkinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(AfxGetApp()->LoadIcon(IDI_NOFITY_ICON3), FALSE);		// 设置小图标
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

	//获取窗口初始时的大小
	CRect rect;
	GetWindowRect(rect);
	m_min_size = rect.Size();

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
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_preview_static.m_hWnd!=NULL && nType!= SIZE_MINIMIZED)
		m_view->MoveWindow(CalculateViewRect());
}


void CSkinDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//限制窗口最小大小
	lpMMI->ptMinTrackSize.x = m_min_size.cx;		//设置最小宽度
	lpMMI->ptMinTrackSize.y = m_min_size.cy;		//设置最小高度

	CDialogEx::OnGetMinMaxInfo(lpMMI);
}
