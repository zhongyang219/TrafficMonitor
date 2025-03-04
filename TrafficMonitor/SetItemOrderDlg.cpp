// SetItemOrderDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "SetItemOrderDlg.h"
#include "afxdialogex.h"


// CSetItemOrderDlg 对话框

IMPLEMENT_DYNAMIC(CSetItemOrderDlg, CBaseDialog)

CSetItemOrderDlg::CSetItemOrderDlg(CWnd* pParent /*=nullptr*/)
    : CBaseDialog(IDD_SELECT_ORDER_DIALOG, pParent), m_item_order(true)
{
    m_item_order.Init();
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

void CSetItemOrderDlg::SetDisplayItem(const DisplayItemSet& display_item)
{
    m_display_item = display_item;
}

DisplayItemSet CSetItemOrderDlg::GetDisplayItem() const
{
    return m_display_item;
}

void CSetItemOrderDlg::SetPluginDisplayItem(const StringSet& plugin_item)
{
    m_plugin_item = plugin_item;
}

const StringSet& CSetItemOrderDlg::GetPluginDisplayItem() const
{
    return m_plugin_item;
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
    ON_CLBN_CHKCHANGE(IDC_LIST1, &CSetItemOrderDlg::OnCheckChanged)
END_MESSAGE_MAP()


// CSetItemOrderDlg 消息处理程序


BOOL CSetItemOrderDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.GetMenuIcon(IDI_ITEM), FALSE);		// 设置小图标

    m_list_ctrl.SetItemHeight(0, DPI(20));
    EnableCtrl(-1);
    ShowItem();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

void CSetItemOrderDlg::ShowItem()
{
    //向列表中添加项目
    m_list_ctrl.ResetContent();
    m_all_displayed_item = m_item_order.GetAllDisplayItemsWithOrder();
    for (const auto& item : m_all_displayed_item)
    {
        m_list_ctrl.AddString(CTaskbarItemOrderHelper::GetItemDisplayName(item));
        if (GetItemChecked(item))
            m_list_ctrl.SetCheck(m_list_ctrl.GetCount() - 1, TRUE);
        else
            m_list_ctrl.SetCheck(m_list_ctrl.GetCount() - 1, FALSE);
    }
}

void CSetItemOrderDlg::EnableCtrl(int list_selected)
{
    EnableDlgCtrl(IDC_MOVE_UP_BUTTON, list_selected > 0 && list_selected < static_cast<int>(m_all_displayed_item.size()));
    EnableDlgCtrl(IDC_MOVE_DOWN_BUTTON, list_selected >= 0 && list_selected < static_cast<int>(m_all_displayed_item.size()) - 1);
}

void CSetItemOrderDlg::EnableDlgCtrl(UINT id, bool enable)
{
    CWnd* pCtrl = GetDlgItem(id);
    if (pCtrl != nullptr)
        pCtrl->EnableWindow(enable);
}

bool CSetItemOrderDlg::GetItemChecked(CommonDisplayItem item)
{
    if (item.is_plugin)
    {
        if (item.plugin_item != nullptr)
            return m_plugin_item.Contains(item.plugin_item->GetItemId());
    }
    else
    {
        return m_display_item.Contains(item.item_type);
    }
    return false;
}

void CSetItemOrderDlg::SaveItemChecked(CommonDisplayItem item, bool checked)
{
    if (item.is_plugin)
    {
        if (item.plugin_item != nullptr)
            m_plugin_item.SetStrContained(item.plugin_item->GetItemId(), checked);
    }
    else
    {
        if (checked)
            m_display_item.Add(item.item_type);
        else
            m_display_item.Remove(item.item_type);
    }
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
        EnableCtrl(cur_index - 1);
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
        EnableCtrl(cur_index + 1);
    }
}


void CSetItemOrderDlg::OnBnClickedRestoreDefaultButton()
{
    // TODO: 在此添加控件通知处理程序代码
    auto item_list{ m_item_order.GetItemOrder() };
    std::sort(item_list.begin(), item_list.end());
    m_item_order.SetOrder(item_list);
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


void CSetItemOrderDlg::OnCheckChanged()
{
    //当用户点击项目前面的复选框时保存该项目的勾选状态
    int cur_index{ m_list_ctrl.GetCurSel() };
    if (cur_index >= 0 && cur_index < static_cast<int>(m_all_displayed_item.size()))
    {
        bool is_checked = (m_list_ctrl.GetCheck(cur_index) != 0);
        CommonDisplayItem item = m_all_displayed_item[cur_index];
        SaveItemChecked(item, is_checked);
    }
}

void CSetItemOrderDlg::OnOK()
{
    // TODO: 在此添加专用代码和/或调用基类

    //保存每个项目的勾选状态
    //auto item_list = m_item_order.GetAllDisplayItemsWithOrder();
    //int i = 0;
    //for (; i < static_cast<int>(item_list.size()); i++)
    //{
    //    bool is_checked = (m_list_ctrl.GetCheck(i) != 0);
    //    CommonDisplayItem item = item_list[i];
    //    SaveItemChecked(item, is_checked);
    //}

    if (m_display_item.IsEmpty() && m_plugin_item.data().empty())
        m_display_item.Add(TDI_UP);

    CBaseDialog::OnOK();
}
