// MainWndColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "MainWndColorDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"


// CMainWndColorDlg 对话框

IMPLEMENT_DYNAMIC(CMainWndColorDlg, CDialog)

CMainWndColorDlg::CMainWndColorDlg(COLORREF colors[MAIN_WND_COLOR_NUM], CWnd* pParent /*=NULL*/)
	: CDialog(IDD_MAIN_COLOR_DIALOG, pParent)
{
	for (int i{}; i < MAIN_WND_COLOR_NUM; i++)
		m_colors[i] = colors[i];
}

CMainWndColorDlg::~CMainWndColorDlg()
{
}

void CMainWndColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CMainWndColorDlg, CDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMainWndColorDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CMainWndColorDlg 消息处理程序


BOOL CMainWndColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
        //初始化列表控件
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() * 2 / 3;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);		//插入第0列
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_COLOR), LVCFMT_LEFT, width1);		//插入第1列
    m_list_ctrl.SetDrawItemRangMargin(theApp.DPI(2));

    //向列表中插入行
    for (int i{}; i < MAIN_WND_COLOR_NUM; i++)
    {
        CString item_name;
        switch (i)
        {
        case 0:
            item_name = CCommon::LoadText(IDS_UPLOAD);
            break;
        case 1:
            item_name = CCommon::LoadText(IDS_DOWNLOAD);
            break;
        case 2:
            item_name = _T("CPU");
            break;
        case 3:
            item_name = CCommon::LoadText(IDS_MEMORY);
            break;
        case 4:
            item_name = CCommon::LoadText(IDS_GPU_DISP);
            break;
#ifndef WITHOUT_TEMPERATURE
        case 5:
            item_name = CCommon::LoadText(IDS_CPU_TEMPERATURE);
            break;
        case 6:
            item_name = CCommon::LoadText(IDS_GPU_TEMPERATURE);
            break;
        case 7:
            item_name = CCommon::LoadText(IDS_HDD_TEMPERATURE);
            break;
        case 8:
            item_name = CCommon::LoadText(IDS_MAINBOARD_TEMPERATURE);
            break;
#endif
        default:
            break;
        }
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            //m_list_ctrl.SetItemText(index, 1, iter->second.c_str());
            m_list_ctrl.SetItemColor(index, 1, m_colors[i]);
        }
    }


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CMainWndColorDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    int index = pNMItemActivate->iItem;
    if (index >= 0 && index < MAIN_WND_COLOR_NUM)
    {
        CMFCColorDialogEx colorDlg(m_colors[index], 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            m_colors[index] = colorDlg.GetColor();
            m_list_ctrl.SetItemColor(index, 1, m_colors[index]);
        }
    }

    *pResult = 0;
}
