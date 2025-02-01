// PluginInfoDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "TrafficMonitor.h"
#include "PluginInfoDlg.h"
#include "FilePathHelper.h"


// CPluginInfoDlg 对话框

IMPLEMENT_DYNAMIC(CPluginInfoDlg, CBaseDialog)

CPluginInfoDlg::CPluginInfoDlg(int plugin_index, CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_NETWORK_INFO_DIALOG, pParent), m_cur_index(plugin_index)
{
    if (m_cur_index < 0 || m_cur_index >= static_cast<int>(theApp.m_plugins.GetPlugins().size()))
        m_cur_index = 0;
}

CPluginInfoDlg::~CPluginInfoDlg()
{
}

void CPluginInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_INFO_LIST1, m_info_list);
}

void CPluginInfoDlg::ShowInfo()
{
    if (m_cur_index >= 0 && m_cur_index < static_cast<int>(theApp.m_plugins.GetPlugins().size()))
    {
        auto& plugin_info = theApp.m_plugins.GetPlugins()[m_cur_index];
        m_info_list.SetItemText(RI_NAME, 1, plugin_info.Property(ITMPlugin::TMI_NAME).c_str());
        m_info_list.SetItemText(RI_DESCRIPTION, 1, plugin_info.Property(ITMPlugin::TMI_DESCRIPTION).c_str());
        m_info_list.SetItemText(RI_FILE_NAME, 1, CFilePathHelper(plugin_info.file_path).GetFileName().c_str());
        m_info_list.SetItemText(RI_FILE_PATH, 1, plugin_info.file_path.c_str());
        m_info_list.SetItemText(RI_ITEM_NUM, 1, std::to_wstring(plugin_info.plugin_items.size()).c_str());
        wstring item_names;
        wstring item_id;
        for (const auto& item : plugin_info.plugin_items)
        {
            item_names += item->GetItemName();
            item_names += L";";
            item_id += item->GetItemId();
            item_id += L";";
        }
        if (!plugin_info.plugin_items.empty())
        {
            item_names.pop_back();
            item_id.pop_back();
        }
        m_info_list.SetItemText(RI_ITEM_NAMES, 1, item_names.c_str());
        m_info_list.SetItemText(RI_ITEM_ID, 1, item_id.c_str());
        m_info_list.SetItemText(RI_AUTHOR, 1, plugin_info.Property(ITMPlugin::TMI_AUTHOR).c_str());
        m_info_list.SetItemText(RI_COPYRIGHT, 1, plugin_info.Property(ITMPlugin::TMI_COPYRIGHT).c_str());
        m_info_list.SetItemText(RI_URL, 1, plugin_info.Property(ITMPlugin::TMI_URL).c_str());
        m_info_list.SetItemText(RI_VERSION, 1, plugin_info.Property(ITMPlugin::TMI_VERSION).c_str());
        std::wstring api_version;
        if (plugin_info.plugin != nullptr)
            api_version = std::to_wstring(plugin_info.plugin->GetAPIVersion());
        m_info_list.SetItemText(RI_API_VERSION, 1, api_version.c_str());
    }

    //显示当前选择指示
    CString str;
    str.Format(_T("%d/%d"), m_cur_index + 1, theApp.m_plugins.GetPlugins().size());
    SetDlgItemText(IDC_INDEX_STATIC, str);
}


BEGIN_MESSAGE_MAP(CPluginInfoDlg, CBaseDialog)
    ON_COMMAND(ID_COPY_TEXT, &CPluginInfoDlg::OnCopyText)
    ON_NOTIFY(NM_RCLICK, IDC_INFO_LIST1, &CPluginInfoDlg::OnNMRClickInfoList1)
    ON_BN_CLICKED(IDC_PREVIOUS_BUTTON, &CPluginInfoDlg::OnBnClickedPreviousButton)
    ON_BN_CLICKED(IDC_NEXT_BUTTON, &CPluginInfoDlg::OnBnClickedNextButton)
    ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()


// CPluginInfoDlg 消息处理程序


BOOL CPluginInfoDlg::OnInitDialog()
{
    CBaseDialog::OnInitDialog();

    // TODO:  在此添加额外的初始化
    SetWindowText(CCommon::LoadText(IDS_PLUGIN_INFO));
    SetIcon(theApp.GetMenuIcon(IDI_PLUGINS), FALSE);		// 设置小图标

    //初始化列表控件
    CRect rect;
    m_info_list.GetClientRect(rect);
    m_info_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
    int width0, width1;
    width0 = rect.Width() / 4;
    width1 = rect.Width() - width0 - theApp.DPI(20) - 1;
    m_info_list.InsertColumn(0, CCommon::LoadText(IDS_ITEM), LVCFMT_LEFT, width0);
    m_info_list.InsertColumn(1, CCommon::LoadText(IDS_VALUE), LVCFMT_LEFT, width1);

    //向列表中插入行
    for (int i = 0; i < RI_MAX; i++)
    {
        m_info_list.InsertItem(i, GetRowName(i));
    }

    //显示列表中的信息
    ShowInfo();

    m_info_list.GetToolTips()->SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

    m_menu.LoadMenu(IDR_INFO_MENU); //装载右键菜单

    return TRUE;  // return TRUE unless you set the focus to a control
                  // 异常: OCX 属性页应返回 FALSE
}

CString CPluginInfoDlg::GetDialogName() const
{
    return _T("PluginInfoDlg");
}

void CPluginInfoDlg::OnCopyText()
{
    if (!CCommon::CopyStringToClipboard(wstring(m_selected_string)))
        MessageBox(CCommon::LoadText(IDS_COPY_TO_CLIPBOARD_FAILED), NULL, MB_ICONWARNING);
}

void CPluginInfoDlg::OnNMRClickInfoList1(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码

    //获取鼠标点击处的文本
    int item, sub_item;
    item = pNMItemActivate->iItem;
    sub_item = pNMItemActivate->iSubItem;
    m_selected_string = m_info_list.GetItemText(item, sub_item);

    //弹出右键菜单
    CMenu* pContextMenu = m_menu.GetSubMenu(0);	//获取第一个弹出菜单
    CPoint point1;	//定义一个用于确定光标位置的位置
    GetCursorPos(&point1);	//获取当前光标的位置，以便使得菜单可以跟随光标
    pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point1.x, point1.y, this); //在指定位置显示弹出菜单

    *pResult = 0;
}

void CPluginInfoDlg::OnBnClickedPreviousButton()
{
    if (m_cur_index > 0)
    {
        m_cur_index--;
        ShowInfo();
    }
}

void CPluginInfoDlg::OnBnClickedNextButton()
{
    if (m_cur_index < static_cast<int>(theApp.m_plugins.GetPlugins().size() - 1))
    {
        m_cur_index++;
        ShowInfo();
    }
}

BOOL CPluginInfoDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_LEFT)
        {
            OnBnClickedPreviousButton();
            return TRUE;
        }
        if (pMsg->wParam == VK_RIGHT)
        {
            OnBnClickedNextButton();
            return TRUE;
        }
    }

    return CBaseDialog::PreTranslateMessage(pMsg);
}


BOOL CPluginInfoDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    //通过鼠标滚轮翻页
    if (zDelta > 0)
    {
        OnBnClickedPreviousButton();
    }
    if (zDelta < 0)
    {
        OnBnClickedNextButton();
    }

    return CBaseDialog::OnMouseWheel(nFlags, zDelta, pt);
}

CString CPluginInfoDlg::GetRowName(int row_index)
{
    switch (row_index)
    {
    case CPluginInfoDlg::RI_NAME:
        return CCommon::LoadText(IDS_NAME);
    case CPluginInfoDlg::RI_DESCRIPTION:
        return CCommon::LoadText(IDS_DESCRIPTION);
    case CPluginInfoDlg::RI_FILE_NAME:
        return CCommon::LoadText(IDS_FILE_NAME);
    case CPluginInfoDlg::RI_FILE_PATH:
        return CCommon::LoadText(IDS_FILE_PATH);
    case CPluginInfoDlg::RI_ITEM_NUM:
        return CCommon::LoadText(IDS_ITEM_NUM);
    case CPluginInfoDlg::RI_ITEM_NAMES:
        return CCommon::LoadText(IDS_ITEM_NAMES);
    case CPluginInfoDlg::RI_ITEM_ID:
        return CCommon::LoadText(IDS_DISP_ITEM_ID);
    case CPluginInfoDlg::RI_AUTHOR:
        return CCommon::LoadText(IDS_AUTHOR);
    case CPluginInfoDlg::RI_COPYRIGHT:
        return CCommon::LoadText(IDS_COPYRIGHT);
    case CPluginInfoDlg::RI_URL:
        return CCommon::LoadText(IDS_URL);
    case CPluginInfoDlg::RI_VERSION:
        return CCommon::LoadText(IDS_VERSION);
    case CPluginInfoDlg::RI_API_VERSION:
        return CCommon::LoadText(IDS_PLUGIN_API_VERSION);
    default:
        break;
    }
    return CString();
}
