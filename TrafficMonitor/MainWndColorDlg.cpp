// MainWndColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "MainWndColorDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"
#include "TrafficMonitorDlg.h"


// CMainWndColorDlg 对话框

IMPLEMENT_DYNAMIC(CMainWndColorDlg, CBaseDialog)

CMainWndColorDlg::CMainWndColorDlg(const std::map<CommonDisplayItem, COLORREF>& colors, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_MAIN_COLOR_DIALOG, pParent), m_colors(colors)
{
}

CMainWndColorDlg::~CMainWndColorDlg()
{
}

CString CMainWndColorDlg::GetDialogName() const
{
    return _T("MainWndColorDlg");
}

void CMainWndColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CMainWndColorDlg, CBaseDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMainWndColorDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CMainWndColorDlg 消息处理程序


BOOL CMainWndColorDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

    SetIcon(theApp.GetMenuIcon(IDI_MAIN_WINDOW), FALSE);		// 设置小图标

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

    static std::set<CommonDisplayItem> all_skin_items;
    CTrafficMonitorDlg::Instance()->GetCurSkin().GetSkinDisplayItems(all_skin_items);

    //向列表中插入行
    for (auto iter = all_skin_items.begin(); iter != all_skin_items.end(); ++iter)
    {
        CString item_name = iter->GetItemName();
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            m_list_ctrl.SetItemColor(index, 1, m_colors[*iter]);
            m_list_ctrl.SetItemData(index, (DWORD_PTR)&(*iter));
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
    COLORREF color = m_list_ctrl.GetItemColor(index, 1);
    CMFCColorDialogEx colorDlg(color, 0, this);
    if (colorDlg.DoModal() == IDOK)
    {
        color = colorDlg.GetColor();
        m_list_ctrl.SetItemColor(index, 1, color);
        CommonDisplayItem* item = (CommonDisplayItem*)(m_list_ctrl.GetItemData(index));
        m_colors[*item] = color;
    }

    *pResult = 0;
}
