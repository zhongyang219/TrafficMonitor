// PluginManagerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "PluginManagerDlg.h"
#include "FilePathHelper.h"


// CPluginManagerDlg 对话框

IMPLEMENT_DYNAMIC(CPluginManagerDlg, CBaseDialog)

CPluginManagerDlg::CPluginManagerDlg(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_PLUGIN_MANAGER_DIALOG, pParent)
{

}

CPluginManagerDlg::~CPluginManagerDlg()
{
}

void CPluginManagerDlg::DoDataExchange(CDataExchange* pDX)
{
    CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}

void CPluginManagerDlg::EnableControl()
{
    bool select_enable{ m_item_selected >= 0 && m_item_selected < m_list_ctrl.GetItemCount() };
    EnableDlgCtrl(IDC_OPTINS_BUTTON, select_enable);
}

CString CPluginManagerDlg::GetDialogName() const
{
    return _T("PluginManagerDlg");
}


BEGIN_MESSAGE_MAP(CPluginManagerDlg, CBaseDialog)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPluginManagerDlg::OnNMRClickList1)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPluginManagerDlg::OnNMClickList1)
    ON_BN_CLICKED(IDC_OPTINS_BUTTON, &CPluginManagerDlg::OnBnClickedOptinsButton)
END_MESSAGE_MAP()


// CPluginManagerDlg 消息处理程序


BOOL CPluginManagerDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetIcon(theApp.GetMenuIcon(IDI_PLUGINS), FALSE);

    //初始化列表控件
    CRect rect;
    m_list_ctrl.GetClientRect(rect);
    m_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 3;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_STATUS), LVCFMT_LEFT, width1);

    //向列表中插入行
    for (const auto& plugin : theApp.m_plugins.GetPlugins())
    {
        std::wstring file_name = CFilePathHelper(plugin.file_path).GetFileName();
        CString status;
        switch (plugin.state)
        {
        case CPluginManager::PluginState::PS_SUCCEED:
            status = _T("加载成功");
            break;
        case CPluginManager::PluginState::PS_MUDULE_LOAD_FAILED:
            status.Format(_T("插件模块加载失败，故障代码：%d"), plugin.error_code);
            break;
        case CPluginManager::PluginState::PS_FUNCTION_GET_FAILED:
            status.Format(_T("函数获取失败，故障代码：%d"), plugin.error_code);
            break;
        }
        int index = m_list_ctrl.GetItemCount();
        m_list_ctrl.InsertItem(index, file_name.c_str());
        m_list_ctrl.SetItemText(index, 1, status);
    }

    EnableControl();

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPluginManagerDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    EnableControl();
    *pResult = 0;
}


void CPluginManagerDlg::OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    EnableControl();
    *pResult = 0;
}


void CPluginManagerDlg::OnBnClickedOptinsButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
    {
        auto plugin = theApp.m_plugins.GetPlugins()[m_item_selected];
        if (plugin.TMPluginOptions != nullptr)
            plugin.TMPluginOptions();
    }
}
