// PluginManagerDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "PluginManagerDlg.h"
#include "FilePathHelper.h"
#include "PluginInfoDlg.h"
#include "WIC.h"


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
    DDX_Control(pDX, IDC_PLUGIN_DOWNLOAD_STATIC, m_plugin_download_lnk);
    DDX_Control(pDX, IDC_PLUGIN_DEV_GUID_STATIC, m_plugin_dev_guide_lnk);
    DDX_Control(pDX, IDC_OPEN_PLUGIN_DIR_STATIC, m_open_plugin_dir_lnk);
}

void CPluginManagerDlg::EnableControl()
{
    bool enable{ IsSelectedPluginEnable() };
    EnableDlgCtrl(IDC_OPTINS_BUTTON, enable);
    EnableDlgCtrl(IDC_PLUGIN_INFO_BUTTON, enable);
}

bool CPluginManagerDlg::IsSelectedValid()
{
    return m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size());
}

bool CPluginManagerDlg::IsSelectedPluginEnable()
{
    CPluginManager::PluginInfo plugin_info{};
    bool plugin_loaded{ false };
    if (IsSelectedValid())
    {
        plugin_info = theApp.m_plugins.GetPlugins()[m_item_selected];
        plugin_loaded = (plugin_info.state == CPluginManager::PluginState::PS_SUCCEED);
    }

    return plugin_loaded;
}

CString CPluginManagerDlg::GetDialogName() const
{
    return _T("PluginManagerDlg");
}


BEGIN_MESSAGE_MAP(CPluginManagerDlg, CBaseDialog)
    ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPluginManagerDlg::OnNMRClickList1)
    ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPluginManagerDlg::OnNMClickList1)
    ON_BN_CLICKED(IDC_OPTINS_BUTTON, &CPluginManagerDlg::OnBnClickedOptinsButton)
    ON_BN_CLICKED(IDC_PLUGIN_INFO_BUTTON, &CPluginManagerDlg::OnBnClickedPluginInfoButton)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CPluginManagerDlg::OnNMDblclkList1)
    ON_WM_INITMENU()
    ON_COMMAND(ID_PLUGIN_DETAIL, &CPluginManagerDlg::OnPluginDetail)
    ON_COMMAND(ID_PLUGIN_OPTIONS, &CPluginManagerDlg::OnPluginOptions)
    ON_COMMAND(ID_PLUGIN_DISABLE, &CPluginManagerDlg::OnPluginDisable)
    ON_MESSAGE(WM_LINK_CLICKED, &CPluginManagerDlg::OnLinkClicked)
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
    int width0, width1, width2;
    width0 = width1 = rect.Width() / 3;
    width2 = rect.Width() - width0 - width1 - theApp.DPI(20) - 1;
    m_list_ctrl.InsertColumn(0, CCommon::LoadText(IDS_FILE_NAME), LVCFMT_LEFT, width0);
    m_list_ctrl.InsertColumn(1, CCommon::LoadText(IDS_PLUGIN_NAME), LVCFMT_LEFT, width1);
    m_list_ctrl.InsertColumn(2, CCommon::LoadText(IDS_STATUS), LVCFMT_LEFT, width2);

    //添加图标
    int item_num = theApp.m_plugins.GetPlugins().size();
    m_plugin_icon_list.Create(theApp.DPI(16), theApp.DPI(16), ILC_COLOR32, item_num, item_num);
    for (const auto& plugin : theApp.m_plugins.GetPlugins())
    {
        HICON hIcon{};
        if (plugin.plugin != nullptr && plugin.plugin->GetAPIVersion() >= 5)
        {
            hIcon = (HICON)plugin.plugin->GetPluginIcon();
        }
        if (hIcon == nullptr)
        {
            if (plugin.state == CPluginManager::PluginState::PS_DISABLE)
                hIcon = theApp.GetMenuIcon(IDI_PLUGIN_DISABLED);
            else
                hIcon = theApp.GetMenuIcon(IDI_PLUGINS);
        }
        m_plugin_icon_list.Add(hIcon);
    }
    m_list_ctrl.SetImageList(&m_plugin_icon_list, LVSIL_SMALL);

    //向列表中插入行
    for (const auto& plugin : theApp.m_plugins.GetPlugins())
    {
        std::wstring file_name = CFilePathHelper(plugin.file_path).GetFileName();
        CString status;
        switch (plugin.state)
        {
        case CPluginManager::PluginState::PS_SUCCEED:
            status = CCommon::LoadText(IDS_PLUGIN_LOAD_SUCCEED);
            break;
        case CPluginManager::PluginState::PS_MUDULE_LOAD_FAILED:
            status = CCommon::LoadTextFormat(IDS_PLUGIN_MODULE_LOAD_FAILED, { static_cast<int>(plugin.error_code) });
            break;
        case CPluginManager::PluginState::PS_FUNCTION_GET_FAILED:
            status = CCommon::LoadTextFormat(IDS_PLUGIN_FUNCTION_GET_FAILED, { static_cast<int>(plugin.error_code) });
            break;
        case CPluginManager::PluginState::PS_DISABLE:
            status = CCommon::LoadText(IDS_DISABLED);
            break;
        case CPluginManager::PluginState::PS_VERSION_NOT_SUPPORT:
            status = CCommon::LoadText(IDS_PLUGIN_VERSION_NOT_SUPPORT);
            break;
        default:
            break;
        }
        int index = m_list_ctrl.GetItemCount();
        m_list_ctrl.InsertItem(index, file_name.c_str(), m_list_ctrl.GetItemCount());
        m_list_ctrl.SetItemText(index, 1, plugin.Property(ITMPlugin::TMI_NAME).c_str());
        m_list_ctrl.SetItemText(index, 2, status);
    }

    m_plugin_download_lnk.SetURL(L"https://github.com/zhongyang219/TrafficMonitorPlugins/blob/main/download/plugin_download.md");
    m_plugin_dev_guide_lnk.SetURL(L"https://github.com/zhongyang219/TrafficMonitor/wiki/%E6%8F%92%E4%BB%B6%E5%BC%80%E5%8F%91%E6%8C%87%E5%8D%97");
    m_open_plugin_dir_lnk.SetLinkIsURL(false);

    EnableControl();

    m_menu.LoadMenu(IDR_PLUGIN_MANAGER_MENU); //装载右键菜单

    //设置菜单图标
    CMenuIcon::AddIconToMenuItem(m_menu.GetSubMenu(0)->GetSafeHmenu(), ID_PLUGIN_DETAIL, FALSE, theApp.GetMenuIcon(IDI_INFO));
    CMenuIcon::AddIconToMenuItem(m_menu.GetSubMenu(0)->GetSafeHmenu(), ID_PLUGIN_OPTIONS, FALSE, theApp.GetMenuIcon(IDI_SETTINGS));

    //设置按钮图标
    SetButtonIcon(IDC_PLUGIN_INFO_BUTTON, theApp.GetMenuIcon(IDI_INFO));
    SetButtonIcon(IDC_OPTINS_BUTTON, theApp.GetMenuIcon(IDI_SETTINGS));

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}


void CPluginManagerDlg::OnNMRClickList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    EnableControl();

    //弹出右键菜单
    CMenu* pContextMenu = m_menu.GetSubMenu(0);	//获取第一个弹出菜单
    pContextMenu->SetDefaultItem(ID_PLUGIN_DETAIL);
    //更新插件子菜单
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
    {
        auto plugin_info = theApp.m_plugins.GetPlugins()[m_item_selected];
        theApp.UpdatePluginMenu(pContextMenu, plugin_info.plugin, 3);
    }

    CPoint point1;	//定义一个用于确定光标位置的位置
    GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

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
        auto plugin_info = theApp.m_plugins.GetPlugins()[m_item_selected];
        if (plugin_info.plugin != nullptr)
        {
            ITMPlugin::OptionReturn rtn = plugin_info.plugin->ShowOptionsDialog(m_hWnd);
            if (rtn == ITMPlugin::OR_OPTION_NOT_PROVIDED)
                MessageBox(CCommon::LoadText(IDS_PLUGIN_NO_OPTIONS_INFO), nullptr, MB_ICONINFORMATION | MB_OK);
            //else if (rtn == ITMPlugin::OR_OPTION_CHANGED)
            //    theApp.m_pMainWnd->SendMessage(WM_REOPEN_TASKBAR_WND);
        }
    }
}


void CPluginManagerDlg::OnBnClickedPluginInfoButton()
{
    // TODO: 在此添加控件通知处理程序代码
    if (IsSelectedPluginEnable())
    {
        CPluginInfoDlg dlg(m_item_selected);
        dlg.DoModal();
    }
}


void CPluginManagerDlg::OnNMDblclkList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    m_item_selected = pNMItemActivate->iItem;
    OnBnClickedPluginInfoButton();
    *pResult = 0;
}


void CPluginManagerDlg::OnInitMenu(CMenu* pMenu)
{
    CBaseDialog::OnInitMenu(pMenu);

    // TODO: 在此处添加消息处理程序代码
    bool enable{ IsSelectedPluginEnable() };
    pMenu->EnableMenuItem(ID_PLUGIN_DETAIL, MF_BYCOMMAND | (enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLUGIN_OPTIONS, MF_BYCOMMAND | (enable ? MF_ENABLED : MF_GRAYED));
    pMenu->EnableMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | (IsSelectedValid() ? MF_ENABLED : MF_GRAYED));

    bool disabled{};
    CPluginManager::PluginInfo plugin_info;
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
    {
        plugin_info = theApp.m_plugins.GetPlugins()[m_item_selected];
        std::wstring file_name = CFilePathHelper(plugin_info.file_path).GetFileName();
        disabled = theApp.m_cfg_data.plugin_disabled.Contains(file_name);
    }
    pMenu->CheckMenuItem(ID_PLUGIN_DISABLE, MF_BYCOMMAND | (disabled ? MF_CHECKED : MF_UNCHECKED));
}


void CPluginManagerDlg::OnPluginDetail()
{
    OnBnClickedPluginInfoButton();
}


void CPluginManagerDlg::OnPluginOptions()
{
    OnBnClickedOptinsButton();
}


void CPluginManagerDlg::OnPluginDisable()
{
    if (m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
    {
        CPluginManager::PluginInfo plugin_info = theApp.m_plugins.GetPlugins()[m_item_selected];
        std::wstring file_name = CFilePathHelper(plugin_info.file_path).GetFileName();
        bool disabled = theApp.m_cfg_data.plugin_disabled.Contains(file_name);
        theApp.m_cfg_data.plugin_disabled.SetStrContained(file_name, !disabled);
        MessageBox(CCommon::LoadText(IDS_RESTART_TO_APPLY_CHANGE_INFO), nullptr, MB_OK | MB_ICONINFORMATION);
    }
}


afx_msg LRESULT CPluginManagerDlg::OnLinkClicked(WPARAM wParam, LPARAM lParam)
{
    CWnd* pCtrl = (CWnd*)wParam;
    //点击了“打开插件目录”
    if (pCtrl == &m_open_plugin_dir_lnk)
    {
        wstring plugin_dir = CCommon::GetModuleDir() + L"plugins";
        CreateDirectory(plugin_dir.c_str(), NULL);       //如果plugins不存在，则创建它
        ShellExecute(NULL, _T("open"), _T("explorer"), plugin_dir.c_str(), NULL, SW_SHOWNORMAL);
    }

    return 0;
}


BOOL CPluginManagerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
    UINT uMsg = LOWORD(wParam);
    //选择了插件命令
    if (uMsg >= ID_PLUGIN_COMMAND_START && uMsg <= ID_PLUGIN_COMMAND_MAX)
    {
        int index = uMsg - ID_PLUGIN_COMMAND_START;
        if (m_item_selected >= 0 && m_item_selected < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
        {
            ITMPlugin* plugin = theApp.m_plugins.GetPlugins()[m_item_selected].plugin;
            if (plugin != nullptr && plugin->GetAPIVersion() >= 5)
            {
                plugin->OnPluginCommand(index, (void*)GetSafeHwnd(), nullptr);
            }
        }
    }

    return CBaseDialog::OnCommand(wParam, lParam);
}
