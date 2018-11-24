// TaskbarColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskbarColorDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"


// CTaskbarColorDlg 对话框

IMPLEMENT_DYNAMIC(CTaskbarColorDlg, CDialog)

CTaskbarColorDlg::CTaskbarColorDlg(COLORREF colors[TASKBAR_COLOR_NUM], CWnd* pParent /*=NULL*/)
	: CDialog(IDD_TASKBAR_COLOR_DIALOG, pParent)
{
	for (int i{}; i < TASKBAR_COLOR_NUM; i++)
		m_colors[i] = colors[i];
}

CTaskbarColorDlg::~CTaskbarColorDlg()
{
}

void CTaskbarColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UP_LABLE_STATIC, m_statics[0]);
	DDX_Control(pDX, IDC_UP_VALUE_STATIC, m_statics[1]);
	DDX_Control(pDX, IDC_DWON_LABLE_STATIC, m_statics[2]);
	DDX_Control(pDX, IDC_DWON_VALUE_STATIC, m_statics[3]);
	DDX_Control(pDX, IDC_CPU_LABLE_STATIC, m_statics[4]);
	DDX_Control(pDX, IDC_CPU_VALUE_STATIC, m_statics[5]);
	DDX_Control(pDX, IDC_MEMORY_LABLE_STATIC, m_statics[6]);
	DDX_Control(pDX, IDC_MEMORY_VALUE_STATIC, m_statics[7]);
}


BEGIN_MESSAGE_MAP(CTaskbarColorDlg, CDialog)
	ON_MESSAGE(WM_STATIC_CLICKED, &CTaskbarColorDlg::OnStaticClicked)
END_MESSAGE_MAP()


// CTaskbarColorDlg 消息处理程序


BOOL CTaskbarColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	for (int i{}; i < TASKBAR_COLOR_NUM; i++)
	{
		m_statics[i].SetFillColor(m_colors[i]);
		m_statics[i].SetLinkCursor();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


afx_msg LRESULT CTaskbarColorDlg::OnStaticClicked(WPARAM wParam, LPARAM lParam)
{
	int item_id = ::GetDlgCtrlID(((CWnd*)wParam)->m_hWnd);
	int index{};
	switch (item_id)
	{
	case IDC_UP_LABLE_STATIC:
		index = 0;
		break;
	case IDC_UP_VALUE_STATIC:
		index = 1;
		break;
	case IDC_DWON_LABLE_STATIC:
		index = 2;
		break;
	case IDC_DWON_VALUE_STATIC:
		index = 3;
		break;
	case IDC_CPU_LABLE_STATIC:
		index = 4;
		break;
	case IDC_CPU_VALUE_STATIC:
		index = 5;
		break;
	case IDC_MEMORY_LABLE_STATIC:
		index = 6;
		break;
	case IDC_MEMORY_VALUE_STATIC:
		index = 7;
		break;
	default:
		return 0;
	}

	CMFCColorDialogEx colorDlg(m_colors[index], 0, this);
	if (colorDlg.DoModal() == IDOK)
	{
		m_colors[index] = colorDlg.GetColor();
		m_statics[index].SetFillColor(m_colors[index]);
	}
	return 0;
}
