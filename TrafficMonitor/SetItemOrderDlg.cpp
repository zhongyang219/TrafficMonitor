// SetItemOrderDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "SetItemOrderDlg.h"
#include "afxdialogex.h"


// CSetItemOrderDlg 对话框

IMPLEMENT_DYNAMIC(CSetItemOrderDlg, CBaseDialog)

CSetItemOrderDlg::CSetItemOrderDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_SELECT_ORDER_DIALOG, pParent)
{

}

CSetItemOrderDlg::~CSetItemOrderDlg()
{
}

void CSetItemOrderDlg::SetItemOrder(const std::vector<int>& item_order)
{
    m_item_order.SetOrder(item_order);
}

const std::vector<int>& CSetItemOrderDlg::GetItemOrder() const
{
    return m_item_order.GetItemOrderConst();
}

void CSetItemOrderDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}


BEGIN_MESSAGE_MAP(CSetItemOrderDlg, CBaseDialog)
    ON_BN_CLICKED(IDC_MOVE_UP_BUTTON, &CSetItemOrderDlg::OnBnClickedMoveUpButton)
    ON_BN_CLICKED(IDC_MOVE_DOWN_BUTTON, &CSetItemOrderDlg::OnBnClickedMoveDownButton)
    ON_BN_CLICKED(IDC_RESTORE_DEFAULT_BUTTON, &CSetItemOrderDlg::OnBnClickedRestoreDefaultButton)
    ON_LBN_SELCHANGE(IDC_LIST1, &CSetItemOrderDlg::OnLbnSelchangeList1)
END_MESSAGE_MAP()


// CSetItemOrderDlg 消息处理程序


BOOL CSetItemOrderDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.GetMenuIcon(IDI_ITEM), FALSE);		// 设置小图标
    EnableCtrl(-1);
    ShowItem();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSetItemOrderDlg::ShowItem()
{
    m_list_ctrl.ResetContent();
    auto item_list = m_item_order.GetAllDisplayItemsWithOrder();
    for (const auto& item : item_list)
    {
        m_list_ctrl.AddString(CTaskbarItemOrderHelper::GetItemDisplayName(item));
    }
}

void CSetItemOrderDlg::EnableCtrl(int list_selected)
{
    int item_count{ static_cast<int>(m_item_order.GetItemOrderConst().size()) };
    EnableDlgCtrl(IDC_MOVE_UP_BUTTON, list_selected > 0 && list_selected < item_count);
    EnableDlgCtrl(IDC_MOVE_DOWN_BUTTON, list_selected >= 0 && list_selected < item_count - 1);
}

void CSetItemOrderDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}


void CSetItemOrderDlg::OnBnClickedMoveUpButton()
{
    // TODO: 在此添加控件通知处理程序代码
    auto& item_list{ m_item_order.GetItemOrder() };
    int cur_index{ m_list_ctrl.GetCurSel() };
    int item_count{ static_cast<int>(item_list.size()) };
    if (cur_index > 0 && cur_index < item_count)
    {
        std::swap(item_list[cur_index], item_list[cur_index - 1]);
        ShowItem();
        m_list_ctrl.SetCurSel(cur_index - 1);
    }
}


void CSetItemOrderDlg::OnBnClickedMoveDownButton()
{
    // TODO: 在此添加控件通知处理程序代码
    auto& item_list{ m_item_order.GetItemOrder() };
    int cur_index{ m_list_ctrl.GetCurSel() };
    int item_count{ static_cast<int>(item_list.size()) };
    if (cur_index >= 0 && cur_index < item_count - 1)
    {
        std::swap(item_list[cur_index], item_list[cur_index + 1]);
        ShowItem();
        m_list_ctrl.SetCurSel(cur_index + 1);
    }
}


void CSetItemOrderDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    auto& item_list{ m_item_order.GetItemOrder() };
    std::sort(item_list.begin(), item_list.end());
    ShowItem();
}

CString CSetItemOrderDlg::GetDialogName() const
{
    return _T("SetItemOrderDlg");
}


void CSetItemOrderDlg::OnLbnSelchangeList1()
{
    // TODO: 在此添加控件通知处理程序代码
    int cur_index{ m_list_ctrl.GetCurSel() };
    EnableCtrl(cur_index);
}
