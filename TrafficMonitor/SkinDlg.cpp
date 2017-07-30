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
}


void CSkinDlg::ShowPreview()
{
	//显示小尺寸的预览图
	m_skin_preview_s.SetWindowPos(&CWnd::wndTop, 0, 0, m_skin_width, m_skin_height_s, SWP_NOMOVE);	//更改控件的宽和高
	m_image = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected]+ L"\\background.bmp").c_str(), IMAGE_BITMAP, m_skin_width, m_skin_height_s, LR_LOADFROMFILE);
	m_skin_preview_s.SetBitmap(m_image);		//为CStatic控件设置图片
	//显示大尺寸的预览图
	m_skin_preview_l.SetWindowPos(&CWnd::wndTop, 0, 0, m_skin_width, m_skin_height_l, SWP_NOMOVE);
	m_image = (HBITMAP)LoadImage(NULL, (m_skins[m_skin_selected] + L"\\background_l.bmp").c_str(), IMAGE_BITMAP, m_skin_width, m_skin_height_l, LR_LOADFROMFILE);
	m_skin_preview_l.SetBitmap(m_image);
	//获取当前皮肤的文字颜色
	m_text_color =  GetPrivateProfileInt(_T("skin"), _T("text_color"), 0, (m_skins[m_skin_selected] + L"\\skin.ini").c_str());
	//获取皮肤作者
	wchar_t buff[64];
	GetPrivateProfileString(_T("skin"), _T("skin_author"), _T("unknow"), buff, 64, (m_skins[m_skin_selected] + L"\\skin.ini").c_str());
	SetDlgItemText(IDC_SKIN_INFO, (wstring(L"皮肤作者：") + buff).c_str());
}

void CSkinDlg::SetPreviewText(CStatic& static_preview)
{
	CDC* pDC = static_preview.GetDC();
	pDC->SetTextColor(m_text_color);
	pDC->SetBkMode(TRANSPARENT);
	pDC->SelectObject(this->GetFont());
	CRect rect;
	static_preview.GetClientRect(&rect);
	rect.left += 10;
	DrawThemeParentBackground(m_hWnd, pDC->GetSafeHdc(), &rect);	//重绘控件区域以解决文字重叠的问题
	pDC->DrawText(_T("示例文本1234ABCD"), rect, DT_VCENTER | DT_SINGLELINE);
	ReleaseDC(pDC);
}


BEGIN_MESSAGE_MAP(CSkinDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CSkinDlg::OnCbnSelchangeCombo1)
	ON_WM_TIMER()
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
	//显示预览图片
	ShowPreview();

	SetTimer(2345, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSkinDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_skin_selected = m_select_box.GetCurSel();
	ShowPreview();
	//显示示例文本
	SetPreviewText(m_skin_preview_s);
	SetPreviewText(m_skin_preview_l);
}


void CSkinDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetPreviewText(m_skin_preview_s);
	SetPreviewText(m_skin_preview_l);
	KillTimer(2345);		//定时器响应一次后就将其销毁

	CDialogEx::OnTimer(nIDEvent);
}
