// TaskbarColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "TaskbarColorDlg.h"
#include "afxdialogex.h"
#include "CMFCColorDialogEx.h"


// CTaskbarColorDlg 对话框

IMPLEMENT_DYNAMIC(CTaskbarColorDlg, CBaseDialog)

CTaskbarColorDlg::CTaskbarColorDlg(const std::map<CommonDisplayItem, TaskbarItemColor>& colors,
    const std::map<CommonDisplayItem, COLORREF>& graph_colors, COLORREF default_graph_color,
    bool enable_text_colors, bool enable_graph_colors, CWnd* pParent /*=NULL*/)
	: CBaseDialog(IDD_TASKBAR_COLOR_DIALOG, pParent), m_colors(colors), m_graph_colors(graph_colors),
    m_default_graph_color(default_graph_color), m_enable_text_colors(enable_text_colors), m_enable_graph_colors(enable_graph_colors)
{
}

CTaskbarColorDlg::~CTaskbarColorDlg()
{
}

CString CTaskbarColorDlg::GetDialogName() const
{
    return _T("TaskbarColorDlg");
}

void CTaskbarColorDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CTaskbarColorDlg, CBaseDialog)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CTaskbarColorDlg::OnNMDblclkList1)
END_MESSAGE_MAP()


// CTaskbarColorDlg 消息处理程序


BOOL CTaskbarColorDlg::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

    SetIcon(theApp.GetMenuIcon(IDI_TASKBAR_WINDOW), FALSE);		// 设置小图标

    //初始化列表控件
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0 = rect.Width() * 2 / 5;
    int color_width = (rect.Width() - width0 - theApp.DPI(20) - 1) / 3;
    int width3 = rect.Width() - width0 - color_width * 2 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_COLOR_LABEL), LVCFMT_LEFT, color_width);
    m_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_COLOR_VALUE), LVCFMT_LEFT, color_width);
    m_list_ctrl.InsertColumn(3, CCommon::LoadText(IDS_COLOR_GRAPH), LVCFMT_LEFT, width3);
    m_list_ctrl.SetDrawItemRangMargin(theApp.DPI(2));

    //向列表中插入行
    for (auto iter = theApp.m_plugins.AllDisplayItemsWithPlugins().begin(); iter != theApp.m_plugins.AllDisplayItemsWithPlugins().end(); ++iter)
    {
        CString item_name = iter->GetItemName();
        if (!item_name.IsEmpty())
        {
            int index = m_list_ctrl.GetItemCount();
            m_list_ctrl.InsertItem(index, item_name);
            auto text_color_iter = m_colors.find(*iter);
            if (text_color_iter != m_colors.end())
            {
                m_list_ctrl.SetItemColor(index, 1, text_color_iter->second.label);
                m_list_ctrl.SetItemColor(index, 2, text_color_iter->second.value);
            }
            auto graph_color_iter = m_graph_colors.find(*iter);
            m_list_ctrl.SetItemColor(index, 3, graph_color_iter == m_graph_colors.end() ? m_default_graph_color : graph_color_iter->second);
            m_list_ctrl.SetItemData(index, (DWORD_PTR)&(*iter));
        }
    }

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}



void CTaskbarColorDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    int index = pNMItemActivate->iItem;
    int col = pNMItemActivate->iSubItem;
    if ((m_enable_text_colors && (col == 1 || col == 2)) || (m_enable_graph_colors && col == 3))
    {
        COLORREF color = m_list_ctrl.GetItemColor(index, col);
        CMFCColorDialogEx colorDlg(color, 0, this);
        if (colorDlg.DoModal() == IDOK)
        {
            color = colorDlg.GetColor();
            m_list_ctrl.SetItemColor(index, col, color);
            CommonDisplayItem* item = (CommonDisplayItem*)(m_list_ctrl.GetItemData(index));
            if (col == 1)
                m_colors[*item].label = color;
            else if (col == 2)
                m_colors[*item].value = color;
            else
                m_graph_colors[*item] = color;
        }
    }

    *pResult = 0;
}
