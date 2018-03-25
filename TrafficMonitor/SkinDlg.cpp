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
	DDX_Control(pDX, IDC_STATIC_SKIN_S, m_skin_preview_s);
	DDX_Control(pDX, IDC_STATIC_SKIN_L, m_skin_preview_l);
	//  DDX_Control(pDX, IDC_STATIC_TEXT1, m_preview_text_s);
	//  DDX_Control(pDX, IDC_STATIC_TEXT2, m_preview_text_l);
	DDX_Control(pDX, IDC_SKIN_COURSE_STATIC, m_skin_course);
	DDX_Control(pDX, IDC_SKIN_DOWNLOAD_STATIC, m_skin_download);
}

void CSkinDlg::GetSkinLayout(const wstring& cfg_path, LayoutData& layout_data)
{
	//从ini文件读取皮肤布局，并根据DPI进行缩放
	layout_data.text_height = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("text_height"), 20, cfg_path.c_str()));
	layout_data.no_text = (GetPrivateProfileInt(_T("layout"), _T("no_text"), 0, cfg_path.c_str()) != 0);

	layout_data.width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("width_l"), 220, cfg_path.c_str()));
	layout_data.height_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("height_l"), 43, cfg_path.c_str()));
	layout_data.up_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_x_l"), 6, cfg_path.c_str()));
	layout_data.up_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_y_l"), 2, cfg_path.c_str()));
	layout_data.up_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("up_width_l"), 108, cfg_path.c_str()));
	layout_data.down_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_x_l"), 114, cfg_path.c_str()));
	layout_data.down_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_y_l"), 2, cfg_path.c_str()));
	layout_data.down_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_width_l"), 110, cfg_path.c_str()));
	layout_data.cpu_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_x_l"), 6, cfg_path.c_str()));
	layout_data.cpu_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_y_l"), 22, cfg_path.c_str()));
	layout_data.cpu_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_width_l"), 108, cfg_path.c_str()));
	layout_data.memory_x_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_x_l"), 114, cfg_path.c_str()));
	layout_data.memory_y_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_y_l"), 22, cfg_path.c_str()));
	layout_data.memory_width_l = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_width_l"), 110, cfg_path.c_str()));
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
	layout_data.down_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_x_s"), 114, cfg_path.c_str()));
	layout_data.down_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_y_s"), 4, cfg_path.c_str()));
	layout_data.down_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("down_width_s"), 110, cfg_path.c_str()));
	layout_data.cpu_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_x_s"), 0, cfg_path.c_str()));
	layout_data.cpu_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_y_s"), 0, cfg_path.c_str()));
	layout_data.cpu_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("cpu_width_s"), 0, cfg_path.c_str()));
	layout_data.memory_x_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_x_s"), 0, cfg_path.c_str()));
	layout_data.memory_y_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_y_s"), 0, cfg_path.c_str()));
	layout_data.memory_width_s = theApp.DPI(GetPrivateProfileInt(_T("layout"), _T("memory_width_s"), 0, cfg_path.c_str()));
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
	GetSkinLayout(cfg_path, m_layout_data);
	//显示小尺寸的预览图
	m_skin_preview_s.SetWindowPos(nullptr, PREVIEW_START_X + m_layout_data.preview_x_s, PREVIEW_START_Y + m_layout_data.preview_y_s, m_layout_data.width_s, m_layout_data.height_s, SWP_NOZORDER);	//更改控件的大小和位置
	m_image = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_s, m_layout_data.height_s, LR_LOADFROMFILE);
	m_skin_preview_s.SetPicture(m_image);		//为CStatic控件设置图片
	//显示大尺寸的预览图
	m_skin_preview_l.SetWindowPos(nullptr, PREVIEW_START_X + m_layout_data.preview_x_l, PREVIEW_START_Y + m_layout_data.preview_y_l, m_layout_data.width_l, m_layout_data.height_l, SWP_NOZORDER);
	m_image = (HBITMAP)LoadImage(NULL, (theApp.m_skin_path + m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, m_layout_data.width_l, m_layout_data.height_l, LR_LOADFROMFILE);
	m_skin_preview_l.SetPicture(m_image);
	//获取当前皮肤的文字颜色
	m_text_color = GetPrivateProfileInt(_T("skin"), _T("text_color"), 0, cfg_path.c_str());
	//获取皮肤作者
#define BUFF_SIZE 64
	wchar_t buff[BUFF_SIZE];
	GetPrivateProfileString(_T("skin"), _T("skin_author"), _T("unknow"), buff, BUFF_SIZE, cfg_path.c_str());
	SetDlgItemText(IDC_SKIN_INFO, (wstring(L"皮肤作者：") + buff).c_str());
	//获取显示文本
	m_disp_str.up = CCommon::GetIniStringW(_T("skin"), _T("up_string"), NONE_STR, cfg_path.c_str());
	m_disp_str.down = CCommon::GetIniStringW(_T("skin"), _T("down_string"), NONE_STR, cfg_path.c_str());
	m_disp_str.cpu = CCommon::GetIniStringW(_T("skin"), _T("cpu_string"), NONE_STR, cfg_path.c_str());
	m_disp_str.memory = CCommon::GetIniStringW(_T("skin"), _T("memory_string"), NONE_STR, cfg_path.c_str());
}


void CSkinDlg::SetPreviewText(CDC* pDC, bool l_preview)
{
	pDC->SetTextColor(m_text_color);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(m_pFont);
	CRect rect;
	CString prev_text;
	CString disp_text;
	if (l_preview)
	{
		//绘制“上传”预览文本
		if (m_layout_data.show_up_l)
		{
			rect.MoveToXY(m_layout_data.up_x_l, m_layout_data.up_y_l);
			rect.right = rect.left + m_layout_data.up_width_l;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.up == NONE_STR) ? theApp.m_main_wnd_data.disp_str.up.c_str() : m_disp_str.up.c_str());
			prev_text.Format(_T("%s88.8KB/s"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“下载”预览文本
		if (m_layout_data.show_down_l)
		{
			rect.MoveToXY(m_layout_data.down_x_l, m_layout_data.down_y_l);
			rect.right = rect.left + m_layout_data.down_width_l;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.down == NONE_STR) ? theApp.m_main_wnd_data.disp_str.down.c_str() : m_disp_str.down.c_str());
			prev_text.Format(_T("%s88.9KB/s"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“CPU”预览文本
		if (m_layout_data.show_cpu_l)
		{
			rect.MoveToXY(m_layout_data.cpu_x_l, m_layout_data.cpu_y_l);
			rect.right = rect.left + m_layout_data.cpu_width_l;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.cpu == NONE_STR) ? theApp.m_main_wnd_data.disp_str.cpu.c_str() : m_disp_str.cpu.c_str());
			prev_text.Format(_T("%s50%%"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“内存”预览文本
		if (m_layout_data.show_memory_l)
		{
			rect.MoveToXY(m_layout_data.memory_x_l, m_layout_data.memory_y_l);
			rect.right = rect.left + m_layout_data.memory_width_l;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.memory == NONE_STR) ? theApp.m_main_wnd_data.disp_str.memory.c_str() : m_disp_str.memory.c_str());
			prev_text.Format(_T("%s51%%"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
	}
	else
	{
		//绘制“上传”预览文本
		if (m_layout_data.show_up_s)
		{
			rect.MoveToXY(m_layout_data.up_x_s, m_layout_data.up_y_s);
			rect.right = rect.left + m_layout_data.up_width_s;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.up == NONE_STR) ? theApp.m_main_wnd_data.disp_str.up.c_str() : m_disp_str.up.c_str());
			prev_text.Format(_T("%s88.8KB/s"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“下载”预览文本
		if (m_layout_data.show_down_s)
		{
			rect.MoveToXY(m_layout_data.down_x_s, m_layout_data.down_y_s);
			rect.right = rect.left + m_layout_data.down_width_s;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.down == NONE_STR) ? theApp.m_main_wnd_data.disp_str.down.c_str() : m_disp_str.down.c_str());
			prev_text.Format(_T("%s88.9KB/s"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“CPU”预览文本
		if (m_layout_data.show_cpu_s)
		{
			rect.MoveToXY(m_layout_data.cpu_x_s, m_layout_data.cpu_y_s);
			rect.right = rect.left + m_layout_data.cpu_width_s;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.cpu == NONE_STR) ? theApp.m_main_wnd_data.disp_str.cpu.c_str() : m_disp_str.cpu.c_str());
			prev_text.Format(_T("%s50%%"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
		//绘制“内存”预览文本
		if (m_layout_data.show_memory_s)
		{
			rect.MoveToXY(m_layout_data.memory_x_s, m_layout_data.memory_y_s);
			rect.right = rect.left + m_layout_data.memory_width_s;
			rect.bottom = rect.top + m_layout_data.text_height;
			if (m_layout_data.no_text)
				disp_text = _T("");
			else
				disp_text = ((m_disp_str.memory == NONE_STR) ? theApp.m_main_wnd_data.disp_str.memory.c_str() : m_disp_str.memory.c_str());
			prev_text.Format(_T("%s51%%"), disp_text);
			pDC->DrawText(prev_text, rect, DT_VCENTER | DT_SINGLELINE);
		}
	}
}


BEGIN_MESSAGE_MAP(CSkinDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSkinDlg::OnCbnSelchangeCombo1)
	//ON_WM_TIMER()
	ON_MESSAGE(WM_CONTROL_REPAINT, &CSkinDlg::OnControlRepaint)
END_MESSAGE_MAP()


// CSkinDlg 消息处理程序

BOOL CSkinDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
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
	//显示预览图片
	ShowPreview();

	m_skin_course.SetURL(_T("https://github.com/zhongyang219/TrafficMonitor/blob/master/皮肤制作教程.md"));
	m_skin_download.SetURL(_T("https://github.com/zhongyang219/TrafficMonitorSkin/blob/master/皮肤下载.md"));

	//SetTimer(2345, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSkinDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_skin_selected = m_select_box.GetCurSel();
	ShowPreview();
	//显示示例文本
	SetPreviewText(m_skin_preview_s.GetDC(), false);
	SetPreviewText(m_skin_preview_l.GetDC(), true);
}


//void CSkinDlg::OnTimer(UINT_PTR nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	SetPreviewText(m_skin_preview_s);
//	SetPreviewText(m_skin_preview_l);
//	KillTimer(2345);		//定时器响应一次后就将其销毁
//
//	CDialogEx::OnTimer(nIDEvent);
//}


afx_msg LRESULT CSkinDlg::OnControlRepaint(WPARAM wParam, LPARAM lParam)
{
	//当接收到预览图控件的重绘消息时，重绘预览文本
	CWnd* pControl = (CWnd*)wParam;
	CDC* pDC = (CDC*)lParam;
	if (pControl == &m_skin_preview_s)
		SetPreviewText(pDC, false);
	else if (pControl == &m_skin_preview_l)
		SetPreviewText(pDC, true);
	return 0;
}
